/** @file led.h
 *  @brief LED initialization prototype and related functional macros
 *
 *  Declaration of the led initialization prototype and macros for LED on
 *  and off functionality
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef LED_H
#define LED_H

#include "msp430.h"

#include "ports.h"

/**
 * @brief Sets all LEDs to off
 *
 */
void init_leds(void);

#define RED_LED_ON (P1OUT |= RED_LED)
#define RED_LED_OFF (P1OUT &= ~RED_LED)
#define GREEN_LED_ON (P6OUT |= GRN_LED)
#define GREEN_LED_OFF (P6OUT &= ~GRN_LED)
#define IR_LED_TOGGLE (P5OUT ^= IR_LED)

#endif /* LED_H */