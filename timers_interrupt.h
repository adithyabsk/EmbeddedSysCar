/** @file timers_interrupt.h
 *  @brief Function prototypes for timer based interrupts
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef TIMERS_INTERRUPT_H
#define TIMERS_INTERRUPT_H

#ifndef TIMERS_ISR_LOCAL_DEF
#define TIMERS_ISR_LOCAL_DEF extern
#endif

typedef enum lf_routine_state_t {
  LF_STATE_MIN,
  INTERCEPTING = LF_STATE_MIN,
  INTERCEPTED,
  WAITING,
  TURNING,
  TURNED,
  FOLLOWING_LINE,
  TURNINTOCIRCLE,
  LF_STATE_MAX = TURNINTOCIRCLE,
  INVALID_LF_STATE
} lf_routine;

/*
 * One time
 */
TIMERS_ISR_LOCAL_DEF volatile char one_time;

/*
 * Count variable for a main loop timer
 */
TIMERS_ISR_LOCAL_DEF volatile unsigned int Time_Sequence;

/*
 * Switch debounce count
 */
TIMERS_ISR_LOCAL_DEF unsigned int switch_debounce_count;

#endif /* TIMERS_INTERRUPT_H */