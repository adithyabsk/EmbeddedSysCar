/** @file drive.c
 *  @brief Implementation for motor drive functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define DRIVE_LOCAL_DEF
#include "drive.h"

#include "msp430.h"

#include "adc.h"

#define BASE_WHEEL_CYCLES (30000U)
#define ALIGNMENT (1000)
#define LEFT_FORWARD_CYCLES (BASE_WHEEL_CYCLES + ALIGNMENT)  // WHEEL_PERIOD
#define RIGHT_FORWARD_CYCLES \
  (BASE_WHEEL_CYCLES - ALIGNMENT)  // (BASE_WHEEL_CYCLES + 15000U)
#define LEFT_REVERSE_CYCLES (BASE_WHEEL_CYCLES)
#define RIGHT_REVERSE_CYCLES (BASE_WHEEL_CYCLES)

#define PID_GAIN (0.7f)
#define PID_LOSS (0.4f)

void stop_drive(void) {
  // Turn off forward and reverse
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

void drive_forward(void) {
  // Turn off reverse
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
  // Turn on forward
  LEFT_FORWARD_SPEED = LEFT_FORWARD_CYCLES;
  RIGHT_FORWARD_SPEED = RIGHT_FORWARD_CYCLES;
}

void drive_reverse(void) {
  // Turn off forward
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  // Turn on reverse
  LEFT_REVERSE_SPEED = LEFT_REVERSE_CYCLES;
  RIGHT_REVERSE_SPEED = RIGHT_REVERSE_CYCLES;
}

void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  LEFT_FORWARD_SPEED = WHEEL_PERIOD;
  // Turn on reverse right
  RIGHT_REVERSE_SPEED = 25000U;
}

void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  RIGHT_FORWARD_SPEED = 25000U;
  // Turn on reverse left
  LEFT_REVERSE_SPEED = WHEEL_PERIOD;
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
