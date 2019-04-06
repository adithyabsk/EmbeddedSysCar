/** @file display_text.h
 *  @brief Display function prototype declarations
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

#include "display.h"

/**
 * @brief Initializes the display variables
 * @note This must be called before any of the other functions
 * can be called.
 *
 */
void init_display(void);

/**
 * @brief Displays a clear screen
 *
 */
void clear_display(void);

/**
 * @brief Displays initial screen
 *
 */
void reset_display(void);

/**
 * @brief Displays the status of both the ADC variables,
 * the IR LED, and a logicla decision whether or not the
 * car is on the left or right side of a black line
 *
 */
void show_adc_status(void);

void show_line_follow_status(void);

void display_baud(void);

void boot_display(void);

void display_scroll(const char[][DISP_TEXT_MAX], int, int, int);

#endif /* DISPLAY_TEXT_H */