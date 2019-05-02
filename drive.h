/** @file drive.h
 *  @brief Function prototypes for drive methods
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DRIVE_H
#define DRIVE_H

#include "msp430.h"

#include "adc.h"
#include "config.h"
#include "display.h"

#ifndef DRIVE_LOCAL_DEF
#define DRIVE_LOCAL_DEF extern
#endif

#define RIGHT_FORWARD_SPEED (TB3CCR1)
#define LEFT_FORWARD_SPEED (TB3CCR2)
#define RIGHT_REVERSE_SPEED (TB3CCR3)
#define LEFT_REVERSE_SPEED (TB3CCR4)
#define WHEEL_PERIOD (256)
#define WHEEL_OFF (0)

enum run_status { RS_WAITING, RS_ARCADE, RS_AUTONOMOUS };

enum drive_state {
  DRIVE_NONE,
  DRIVE_FORWARD,
  DRIVE_REVERSE,
  DRIVE_LEFT,
  DRIVE_RIGHT,
  DRIVE_INVALID
};

enum auton_states {
  BL_START,
  INTERCEPT,
  BL_TRAVEL,
  BL_CIRCLE,
  BL_EXIT,
  BL_STOP,
  BL_NONE,
  BL_MAX = BL_NONE
};

enum checkpoint {
  CHECK_0,
  CHECK_1,
  CHECK_2,
  CHECK_3,
  CHECK_4,
  CHECK_5,
  CHECK_6,
  CHECK_7,
  CHECK_8,
  CHECK_NONE,
  CHECK_LIST_MAX = CHECK_NONE
};

DRIVE_LOCAL_DEF enum auton_states as;

DRIVE_LOCAL_DEF enum drive_state car_drive_state;
DRIVE_LOCAL_DEF unsigned int sched_drive_time;
DRIVE_LOCAL_DEF enum drive_state sched_drive_state;

DRIVE_LOCAL_DEF struct config_value f_align;
DRIVE_LOCAL_DEF struct config_value r_align;
DRIVE_LOCAL_DEF struct config_value p_gain;
DRIVE_LOCAL_DEF struct config_value d_gain;

DRIVE_LOCAL_DEF enum run_status car_run_status;
DRIVE_LOCAL_DEF enum checkpoint curr_checkpoint;
DRIVE_LOCAL_DEF unsigned int drive_start_time;

DRIVE_LOCAL_DEF unsigned int is_arcade;

void increment_checkpoint(void);

/**
 * @brief Stops all car motion
 *
 */
void stop_drive(void);

/**
 * @brief Sets the cars motors to drive forwards
 * @note DO NOT RUN BOTH THIS METHOD AND ANY OTHER DRIVE METHOD CONCURRENTLY
 *
 */
void drive_forward(void);

/**
 * @brief Sets the cars motors to drive backwards
 * @note DO NOT RUN BOTH THIS METHOD AND ANY OTHER DRIVE METHOD CONCURRENTLY
 *
 */
void drive_reverse(void);

/**
 * @brief Sets the cars motors to spin clockwise
 * @note DO NOT RUN BOTH THIS METHOD AND ANY OTHER DRIVE METHOD CONCURRENTLY
 *
 */
void drive_cw(void);

/**
 * @brief Sets the cars motors to spin counter-clockwise
 * @note DO NOT RUN BOTH THIS METHOD AND ANY OTHER DRIVE METHOD CONCURRENTLY
 *
 */
void drive_ccw(void);

void forward_turn(int cycle_offset);

void update_speeds(void);

void run_controller(void);

// Initialize the state variables and config vars for the car
extern inline void init_drive(void);
extern inline void init_drive_config(void);

void set_drive_stop(void);

void set_drive_forward(void);

void set_drive_reverse(void);

void set_drive_left(void);

void set_drive_right(void);

void follow_line_state_controller(void);

void init_auto_setup(void);

void set_arcade_drive(void);
void set_auton_drive(void);

void main_drive_controller(void);

void decrement_checkpoint(void);

void start_auto(void);
void stop_auto(void);

#endif /* DRIVE_H */