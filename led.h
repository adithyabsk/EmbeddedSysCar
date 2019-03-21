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

#define IR_LED_TOGGLE (P5OUT ^= IR_LED)

/**
 * @brief Cyclical blinking pattern for Red Green LEDs
 *
 */
void process_leds(void);

#endif /* LED_H */