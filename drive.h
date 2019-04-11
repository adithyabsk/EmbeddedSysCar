/** @file drive.h
 *  @brief Function prototypes for drive methods
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DRIVE_H
#define DRIVE_H

#include "msp430.h"

#include "adc.h"

#ifndef DRIVE_LOCAL_DEF
#define DRIVE_LOCAL_DEF extern
#endif

#define RIGHT_FORWARD_SPEED (TB3CCR1)
#define LEFT_FORWARD_SPEED (TB3CCR2)
#define RIGHT_REVERSE_SPEED (TB3CCR3)
#define LEFT_REVERSE_SPEED (TB3CCR4)
#define WHEEL_PERIOD (256)
#define WHEEL_OFF (0)

enum drive_state {
  DRIVE_NONE,
  DRIVE_FORWARD,
  DRIVE_REVERSE,
  DRIVE_LEFT,
  DRIVE_RIGHT,
  DRIVE_INVALID
};

DRIVE_LOCAL_DEF volatile enum follow_line_state prev_fl_state;
DRIVE_LOCAL_DEF volatile unsigned int fl_timer_counter;

DRIVE_LOCAL_DEF enum drive_state car_drive_state;
DRIVE_LOCAL_DEF unsigned int sched_drive_time;
DRIVE_LOCAL_DEF enum drive_state sched_drive_state;

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

void init_drive(void);
void arcade_drive_state_machine(void);
void schedule_drive_state(void);

#endif /* DRIVE_H */