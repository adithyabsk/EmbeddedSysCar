/** @file drive.h
 *  @brief Function prototypes for drive methods
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DRIVE_H
#define DRIVE_H

#include "msp430.h"

#define RIGHT_FORWARD_SPEED (TB3CCR1)
#define LEFT_FORWARD_SPEED (TB3CCR2)
#define RIGHT_REVERSE_SPEED (TB3CCR3)
#define LEFT_REVERSE_SPEED (TB3CCR4)
#define WHEEL_PERIOD (40000)
#define WHEEL_OFF (0)

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

#endif /* DRIVE_H */