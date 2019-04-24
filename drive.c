/** @file drive.c
 *  @brief Implementation for motor drive functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define DRIVE_LOCAL_DEF
#include "drive.h"

#include "msp430.h"

#include "adc.h"
#include "common.h"
#include "config.h"
#include "scheduler.h"
#include "timers.h"

#define BASE_WHEEL_CYCLES (256U)
#define ALIGNMENT (10)
#define LEFT_FORWARD_CYCLES (BASE_WHEEL_CYCLES)
#define RIGHT_FORWARD_CYCLES (BASE_WHEEL_CYCLES)
#define LEFT_REVERSE_CYCLES (BASE_WHEEL_CYCLES)
#define RIGHT_REVERSE_CYCLES (BASE_WHEEL_CYCLES)

#define PID_GAIN (0.7f)
#define PID_LOSS (0.4f)
#define WHEEL_SPLIT (2)

enum drive_state prev_drive_state = DRIVE_NONE;

inline void init_drive(void) {
  init_config(&forward_alignment, (int)(-WHEEL_PERIOD / WHEEL_SPLIT),
              (int)(WHEEL_PERIOD / WHEEL_SPLIT), INIT_CLEAR);
  init_config(&reverse_alignment, (int)(-WHEEL_PERIOD / WHEEL_SPLIT),
              (int)(WHEEL_PERIOD / WHEEL_SPLIT), INIT_CLEAR);

  car_drive_state = DRIVE_NONE;
  car_run_status = RS_WAITING;
  curr_checkpoint = CHECK_0;
  drive_start_time = INIT_CLEAR;
}

void stop_drive(void) {
  // Turn off forward and reverse
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

void drive_forward(void) {
  stop_drive();

  // Turn on forward
  LEFT_FORWARD_SPEED = WHEEL_PERIOD;
  // (unsigned int)(WHEEL_PERIOD + forward_alignment.curr_val);
  RIGHT_FORWARD_SPEED = WHEEL_PERIOD;
  // (unsigned int)(WHEEL_PERIOD - forward_alignment.curr_val);
}

void drive_reverse(void) {
  stop_drive();

  // Turn on reverse
  LEFT_REVERSE_SPEED =
      (unsigned int)(WHEEL_PERIOD + reverse_alignment.curr_val);
  RIGHT_REVERSE_SPEED =
      (unsigned int)(WHEEL_PERIOD - reverse_alignment.curr_val);
}

void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  LEFT_FORWARD_SPEED =
      (unsigned int)((WHEEL_PERIOD + forward_alignment.curr_val) / WHEEL_SPLIT);
  // Turn on reverse right
  RIGHT_REVERSE_SPEED =
      (unsigned int)((WHEEL_PERIOD - reverse_alignment.curr_val) / WHEEL_SPLIT);
}

void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  RIGHT_FORWARD_SPEED =
      (unsigned int)((WHEEL_PERIOD - forward_alignment.curr_val) / WHEEL_SPLIT);
  // Turn on reverse left
  LEFT_REVERSE_SPEED =
      (unsigned int)((WHEEL_PERIOD + reverse_alignment.curr_val) / WHEEL_SPLIT);
}

void forward_turn(int cycle_offset) {
  // stop all motion
  // stop_drive();

  // positive turn left, negative turn right
  // that means positive add to right and subtract from left
  int computed_left = LEFT_FORWARD_CYCLES - cycle_offset;
  int computed_right = RIGHT_FORWARD_CYCLES + cycle_offset;

  // Set variables
  LEFT_FORWARD_SPEED = computed_left;
  RIGHT_FORWARD_SPEED = computed_right;
}

void reverse_turn(int cycle_offset) {
  // stop all motion
  stop_drive();
  // positive reverse right, negative reverse left
  // that means positive add to left subtract from right
  int computed_left = LEFT_REVERSE_CYCLES + cycle_offset;
  int computed_right = RIGHT_REVERSE_CYCLES - cycle_offset;

  // Set variables
  LEFT_REVERSE_SPEED = computed_left;
  RIGHT_REVERSE_SPEED = computed_right;
}

void set_drive_stop(void) { car_drive_state = DRIVE_NONE; }

void set_drive_forward(void) { car_drive_state = DRIVE_FORWARD; }

void set_drive_reverse(void) { car_drive_state = DRIVE_REVERSE; }

void set_drive_left(void) { car_drive_state = DRIVE_LEFT; }

void set_drive_right(void) { car_drive_state = DRIVE_RIGHT; }

void arcade_drive_state_machine(void) {
  static int has_updated_rs = INIT_CLEAR;
  if (car_drive_state != prev_drive_state) {
    if (!has_updated_rs) {
      has_updated_rs = BOOLEAN_TRUE;
      car_run_status = RS_ARCADE;
      drive_start_time = wall_clock_time_count;
    }
    switch (car_drive_state) {
      case DRIVE_NONE:
        stop_drive();
        break;
      case DRIVE_FORWARD:
        drive_forward();
        break;
      case DRIVE_REVERSE:
        drive_reverse();
        break;
      case DRIVE_LEFT:
        drive_ccw();
        break;
      case DRIVE_RIGHT:
        drive_cw();
        break;
      default:
        stop_drive();
        break;
    }
    prev_drive_state = car_drive_state;
  }
}

unsigned int compute_speed(int cycles, int should_add, float mult,
                           int counter) {
  unsigned int temp_comp;
  if (should_add) {
    temp_comp = (unsigned int)(cycles + cycles * mult * 0.5 * counter);
  } else {
    temp_comp = (unsigned int)(cycles - cycles * mult * 0.5 * counter);
  }
  if (should_add && (temp_comp < cycles)) {
    return WHEEL_PERIOD;
  } else if ((!should_add && (temp_comp > cycles)) | (temp_comp < 13000)) {
    return 13000;
  } else {
    return temp_comp;
  }
}

void update_speeds(void) {
  unsigned int computed_left, computed_right;
  static int last_known_state = SIDEWAYS;
  switch (fl_state) {
    case NO_LINE:
      switch (last_known_state) {
        case LEFT_OF_LINE:
          computed_left =
              compute_speed(LEFT_FORWARD_CYCLES, 1, PID_GAIN, fl_timer_counter);
          computed_right = compute_speed(RIGHT_FORWARD_CYCLES, 0, PID_LOSS,
                                         fl_timer_counter);
          break;
        case RIGHT_OF_LINE:
          computed_left =
              compute_speed(LEFT_FORWARD_CYCLES, 0, PID_LOSS, fl_timer_counter);
          computed_right = compute_speed(RIGHT_FORWARD_CYCLES, 1, PID_GAIN,
                                         fl_timer_counter);
          break;
        default:
          computed_left = LEFT_FORWARD_CYCLES;
          computed_right = RIGHT_FORWARD_CYCLES;
          break;
      }
      break;
    case SIDEWAYS:
      computed_left = LEFT_FORWARD_CYCLES;
      computed_right = RIGHT_FORWARD_CYCLES;
      break;
    case LEFT_OF_LINE:
      computed_left =
          compute_speed(LEFT_FORWARD_CYCLES, 1, PID_GAIN, fl_timer_counter);
      computed_right =
          compute_speed(RIGHT_FORWARD_CYCLES, 0, PID_LOSS, fl_timer_counter);
      break;
    case RIGHT_OF_LINE:
      computed_left =
          compute_speed(LEFT_FORWARD_CYCLES, 0, PID_LOSS, fl_timer_counter);
      computed_right =
          compute_speed(RIGHT_FORWARD_CYCLES, 1, PID_GAIN, fl_timer_counter);
      break;
    default:
      break;
  }
  fl_timer_counter++;

  // Define volatile access order
  enum follow_line_state _fl_state = fl_state;
  enum follow_line_state _prev_fl_state = prev_fl_state;

  if (_fl_state != _prev_fl_state) {
    fl_timer_counter = 1;
  }
  if (_fl_state != NO_LINE) {
    last_known_state = _fl_state;
  }
  prev_fl_state = _fl_state;

  LEFT_FORWARD_SPEED = computed_left;
  RIGHT_FORWARD_SPEED = computed_right;
}
