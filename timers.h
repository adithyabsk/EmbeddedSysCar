/** @file timers.h
 *  @brief Function prototypes for system timers
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef TIMERS_H
#define TIMERS_H

#ifndef TIMERS_LOCAL_DEF
#define TIMERS_LOCAL_DEF extern
#endif

#include "msp430.h"

// Timer B0
// Calculation SMCLK / 2 / 8 / (1/x in seconds) --> rate
#define TB0CCR0_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms
#define TB0CCR1_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms
#define TB0CCR2_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms

// Timer B1
#define TB1CCR0_INTERVAL (6250)   // 8,000,000/8/8/[1/50msec] --> 50ms
#define TB1CCR1_INTERVAL (1250)   // 8,000,000/8/8/[1/10msec] --> 10ms
#define TB1CCR2_INTERVAL (25000)  // 8,000,000/8/8/[1/200msec] --> 200ms

enum lf_routine {
  INTERCEPTING,
  INTERCEPTED,
  WAITING,
  TURNING,
  TURNED,
  FOLLOWING_LINE,
  TURNINTOCIRCLE,
  INVALID_LF_STATE
};

TIMERS_LOCAL_DEF volatile char one_time;
TIMERS_LOCAL_DEF volatile unsigned int Time_Sequence;
TIMERS_LOCAL_DEF unsigned int switch_debounce_count;
TIMERS_LOCAL_DEF volatile unsigned int wall_clock_time_count;
TIMERS_LOCAL_DEF volatile enum lf_routine lf_routine_state;

/**
 * @brief Initializes all of the system timers
 *
 * This inclues TimerB0, TimerB1, TimerB2, and TimerB3
 *
 */
void init_timers(void);

#endif /* TIMERS_H */