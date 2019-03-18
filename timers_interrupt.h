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