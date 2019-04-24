/** @file display_text.h
 *  @brief Display function prototype declarations
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

#include "display.h"

#define TIME_VAL_OFFSET (4)

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

void display_run_status(void);

void int2hex4bit(int, int, char*);

void state2str(int, int, char*);

void walltime2dec(int, int, char*);

void loopback_test_display(char*, char*, int);

void display_scroll(const char[][DISP_TEXT_SIZE], int, int, int);

void display_iot_data(char[][DISP_TEXT_SIZE], int, int);

void truncate_disp_str(char[DISP_TEXT_SIZE], char*);

#endif /* DISPLAY_TEXT_H */