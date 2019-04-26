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
#include "ports.h"
#include "scheduler.h"
#include "timers.h"

#define BASE_WHEEL_CYCLES (230)
#define BASE_TURN_CYCLES (200)
#define ALIGNMENT (10)
#define FORWARD_CYCLES (BASE_WHEEL_CYCLES)
#define REVERSE_CYCLES (BASE_WHEEL_CYCLES)

#define KP_GAIN (500)
#define WHEEL_SPLIT (2)

#define IR_TOLERANCE (0x100)
#define BLACK (0xA00)
#define MAX_IR_DIFF (2000)

enum follow_line_state { ON_WHITE, ON_LINE, LEFT_OF_LINE, RIGHT_OF_LINE };

enum follow_line_state fl_state;
enum drive_state prev_drive_state = DRIVE_NONE;

static int emitter_enabled;
static int should_line_follow;
static int has_scheduled;
static float pid_last_error;
static int has_updated_rs;

inline void init_drive_config(void) {
  init_config(&f_align, (int)(-WHEEL_PERIOD / WHEEL_SPLIT),
              (int)(WHEEL_PERIOD / WHEEL_SPLIT), INIT_CLEAR);
  init_config(&r_align, (int)(-WHEEL_PERIOD / WHEEL_SPLIT),
              (int)(WHEEL_PERIOD / WHEEL_SPLIT), INIT_CLEAR);
  init_config(&p_gain, 0, 4000, 1600);
  init_config(&d_gain, 0, 100, 5);
}

inline void init_drive(void) {
  car_drive_state = DRIVE_NONE;
  car_run_status = RS_WAITING;
  curr_checkpoint = CHECK_0;
  drive_start_time = INIT_CLEAR;

  is_arcade = BOOLEAN_TRUE;
  has_updated_rs = INIT_CLEAR;
}

void increment_checkpoint(void) {
  if (curr_checkpoint < 8) {
    curr_checkpoint++;
  }
}

void decrement_checkpoint(void) {
  if (curr_checkpoint > 0) {
    curr_checkpoint--;
  }
}

static unsigned int protect_wheel(int set_val) {
  if (set_val < INIT_CLEAR) {
    return (unsigned int)INIT_CLEAR;
  } else if (set_val > WHEEL_PERIOD) {
    return (unsigned int)(WHEEL_PERIOD);
  } else {
    return (unsigned int)set_val;
  }
}

VOID_FUNC_PTR stop_drive_ptr = &stop_drive;
void stop_drive(void) {
  // Turn off forward and reverse
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

VOID_FUNC_PTR drive_forward_ptr = &drive_forward;
void drive_forward(void) {
  stop_drive();

  // Turn on forward
  LEFT_FORWARD_SPEED = protect_wheel(FORWARD_CYCLES + f_align.curr_val);
  RIGHT_FORWARD_SPEED = protect_wheel(FORWARD_CYCLES - f_align.curr_val);
}

VOID_FUNC_PTR drive_reverse_ptr = &drive_reverse;
void drive_reverse(void) {
  stop_drive();

  // Turn on reverse
  LEFT_REVERSE_SPEED = protect_wheel(REVERSE_CYCLES + r_align.curr_val);
  RIGHT_REVERSE_SPEED = protect_wheel(REVERSE_CYCLES - r_align.curr_val);
}

VOID_FUNC_PTR drive_cw_ptr = &drive_cw;
void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  LEFT_FORWARD_SPEED = protect_wheel((BASE_TURN_CYCLES + f_align.curr_val));
  // Turn on reverse right
  RIGHT_REVERSE_SPEED = protect_wheel(BASE_TURN_CYCLES - r_align.curr_val);
}

VOID_FUNC_PTR drive_ccw_ptr = &drive_ccw;
void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  RIGHT_FORWARD_SPEED = protect_wheel(BASE_TURN_CYCLES - f_align.curr_val);
  // Turn on reverse left
  LEFT_REVERSE_SPEED = protect_wheel((BASE_TURN_CYCLES + r_align.curr_val));
}

void forward_turn(int cycle_offset) {
  // stop all motion
  stop_drive();

  // positive turn left, negative turn right
  // that means positive add to right and subtract from left
  int computed_left =
      protect_wheel((FORWARD_CYCLES + f_align.curr_val) - cycle_offset);
  int computed_right =
      protect_wheel((FORWARD_CYCLES - f_align.curr_val) + cycle_offset);

  // Set variables
  LEFT_FORWARD_SPEED = computed_left;
  RIGHT_FORWARD_SPEED = computed_right;
}

void reverse_turn(int cycle_offset) {
  // stop all motion
  stop_drive();

  // positive turn left, negative turn right
  // that means positive add to right and subtract from left
  int computed_left =
      protect_wheel((REVERSE_CYCLES + r_align.curr_val) - cycle_offset);
  int computed_right =
      protect_wheel((REVERSE_CYCLES - r_align.curr_val) + cycle_offset);

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

static int get_detector_diff(void) {
  int _ldet = (int)adc_ldet;
  int _rdet = (int)adc_rdet;

  return (_ldet - _rdet);
}

enum follow_line_state get_curr_lf_state(void) {
  int left = adc_ldet;
  int right = adc_rdet;

  if ((right < BLACK) && (left < BLACK)) {
    return ON_WHITE;
  } else if ((right > BLACK) && (left > BLACK)) {
    return ON_LINE;
  } else if ((right > BLACK) && (left < BLACK)) {
    return RIGHT_OF_LINE;
  } else {
    return LEFT_OF_LINE;
  }
}

void follow_line(void) {
  int curr_pos = get_detector_diff();
  float error = (float)(curr_pos - INIT_CLEAR) / (float)(MAX_IR_DIFF);
  float derivative = error - pid_last_error;
  float turn =
      (float)(p_gain.curr_val) * error +
      (float)(d_gain.curr_val) * derivative;  // scaled between -1 and 1
  forward_turn((int)turn);
  pid_last_error = error;
}

void update_fl_state(void) {
  static enum follow_line_state prev_fl_state = ON_WHITE;
  enum follow_line_state _fl_state = get_curr_lf_state();
  switch (_fl_state) {
    case LEFT_OF_LINE:
    case RIGHT_OF_LINE:
      prev_fl_state = _fl_state;
      fl_state = ON_LINE;
      break;
    case ON_LINE:
      fl_state = ON_LINE;
      break;
    case ON_WHITE:
      fl_state = prev_fl_state;
      break;
    default:
      break;
  }
}

void follow_line_state_controller(void) {
  update_fl_state();
  switch (fl_state) {
    case ON_WHITE:
      drive_forward();
      break;
    case ON_LINE:
      follow_line();
      break;
    case LEFT_OF_LINE:
      forward_turn(60);
      break;
    case RIGHT_OF_LINE:
      forward_turn(-60);
      break;
    default:
      break;
  }
}

static int find_line(void) {
  switch (get_curr_lf_state()) {
    case ON_LINE:
      return BOOLEAN_TRUE;
      break;
    default:
      return BOOLEAN_FALSE;
      break;
  }
}

void init_auto_setup(void) {
  as = BL_NONE;
  emitter_enabled = BOOLEAN_FALSE;
  should_line_follow = BOOLEAN_FALSE;
  has_scheduled = BOOLEAN_FALSE;
  pid_last_error = INIT_CLEAR;

  start_auto();
}

static void drive_left_arc(void) { forward_turn(-1); }
VOID_FUNC_PTR drive_left_arc_ptr = &drive_left_arc;

static void reverse_arc(void) { reverse_turn(-70); }
VOID_FUNC_PTR reverse_arc_ptr = &reverse_arc;

static void set_emitter(void) { emitter_enabled = BOOLEAN_TRUE; }
VOID_FUNC_PTR set_emitter_ptr = &set_emitter;

static void enable_emitter(void) {
  IR_LED_ON;
  schedule_func_call(set_emitter_ptr, 5);
}
VOID_FUNC_PTR enable_emitter_ptr = &enable_emitter;

static void enable_line_follow(void) { should_line_follow = BOOLEAN_TRUE; }
VOID_FUNC_PTR enable_line_follow_ptr = &enable_line_follow;

static void set_auto_intercept2(void) {
  as = BL_CIRCLE;
  stop_drive();
  should_line_follow = BOOLEAN_FALSE;
  has_scheduled = BOOLEAN_FALSE;
}
VOID_FUNC_PTR set_auto_intercept2_ptr = &set_auto_intercept2;

static void set_auto_stop(void) {
  stop_drive();
  as = BL_STOP;
}
VOID_FUNC_PTR set_auto_stop_ptr = &set_auto_stop;

static void autonomous_control(void) {
  int _line_test = find_line();
  switch (as) {
    case BL_NONE:
      break;
    case BL_START:
      if (!has_scheduled) {
        has_scheduled = BOOLEAN_TRUE;
        schedule_func_call(reverse_arc_ptr, 0);
        schedule_func_call(drive_left_arc_ptr, 10);
        schedule_func_call(enable_emitter_ptr, 35);
      }
      if (emitter_enabled && _line_test) {
        has_scheduled = BOOLEAN_FALSE;
        stop_drive();
        as = INTERCEPT;
      }
      break;
    case INTERCEPT:
    case BL_TRAVEL:
      if (!has_scheduled) {
        has_scheduled = BOOLEAN_TRUE;
        schedule_func_call(drive_ccw_ptr, 10);  // should be 2 seconds
        schedule_func_call(enable_line_follow_ptr, 14);
        schedule_func_call(set_auto_intercept2_ptr, 20);

        // schedule state change to bl travel
        // schedule intercept 2
      }
      if (should_line_follow) {
        if (as != BL_TRAVEL) {
          as = BL_TRAVEL;
        }
        follow_line_state_controller();
      }
      // follow line and time this process --> go to next state
      break;
    case BL_CIRCLE:
      if (!has_scheduled) {
        has_scheduled = BOOLEAN_TRUE;
        schedule_func_call(enable_line_follow_ptr, 10);
      }
      if (should_line_follow) {
        if (as != BL_CIRCLE) {
          as = BL_CIRCLE;
        }
        follow_line_state_controller();
      }
      break;
    case BL_EXIT:
      if (!has_scheduled) {
        has_scheduled = BOOLEAN_TRUE;
        schedule_func_call(drive_forward_ptr, 10);
        schedule_func_call(set_auto_stop_ptr, 35);
      }
      // turn, drive 2 feet exit the circle --> go to next state
      break;
    case BL_STOP:
      break;
    default:
      break;
  }
}

void main_drive_controller(void) {
  switch (car_run_status) {
    case RS_WAITING:
    case RS_ARCADE:
      arcade_drive_state_machine();
      break;
    case RS_AUTONOMOUS:
      autonomous_control();
      break;
    default:
      break;
  }
}

void set_arcade_drive(void) {
  stop_drive();
  init_drive();
}

void set_auton_drive(void) {
  stop_drive();
  init_auto_setup();
  car_run_status = RS_AUTONOMOUS;
}

void stop_auto(void) {
  if (as == BL_CIRCLE) {
    as = BL_EXIT;
    stop_drive();
    has_scheduled = BOOLEAN_FALSE;
  }
}

void start_auto(void) {
  if (as == BL_NONE) {
    as = BL_START;
  }
}
