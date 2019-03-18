/** @file timers.h
 *  @brief Function prototypes for system timers
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef TIMERS_H
#define TIMERS_H

// Timer B0
// Calculation SMCLK / 2 / 8 / (1/x in seconds) --> rate
#define TB0CCR0_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms
#define TB0CCR1_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms
#define TB0CCR2_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50ms

// Timer B1
#define TB1CCR0_INTERVAL (62500)  // 8,000,000/8/8/[1/50msec] --> 50ms

/**
 * @brief Initializes all of the system timers
 *
 * This inclues TimerB0, TimerB1, TimerB2, and TimerB3
 *
 */
void init_timers(void);

#endif /* TIMERS_H */