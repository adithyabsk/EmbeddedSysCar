/** @file display.c
 *  @brief Raw display functionality
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define DISPLAY_LOCAL_DEF
#include "display.h"

#include <string.h>

#include "msp430.h"

#include "common.h"

#define DISP_0 (0)
#define DISP_1 (1)
#define DISP_2 (2)
#define DISP_3 (3)
#define DISPL_START (0)

// Library LCD variable access
extern char display_line[DISP_MAX_ROWS][DISP_TEXT_SIZE];
extern char* display[DISP_MAX_ROWS];
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;

inline void init_display(void) {
  int i;

  for (i = INIT_CLEAR; i < DISP_TEXT_SIZE; i++) {
    display_line[DISP_0][i] = INIT_CLEAR;
    display_line[DISP_1][i] = INIT_CLEAR;
    display_line[DISP_2][i] = INIT_CLEAR;
    display_line[DISP_3][i] = INIT_CLEAR;
  }
  display[DISP_0] = &display_line[DISP_0][DISPL_START];
  display[DISP_1] = &display_line[DISP_1][DISPL_START];
  display[DISP_2] = &display_line[DISP_2][DISPL_START];
  display[DISP_3] = &display_line[DISP_3][DISPL_START];

  update_display = INIT_CLEAR;
  update_display_count = INIT_CLEAR;
}

void set_clear_lines(void) {
  strcpy(display_line[DISP_0], EMPTY_STR);
  strcpy(display_line[DISP_1], EMPTY_STR);
  strcpy(display_line[DISP_2], EMPTY_STR);
  strcpy(display_line[DISP_3], EMPTY_STR);
}

void update_lines(void) {
  update_string(display_line[DISP_0], DISP_0);
  update_string(display_line[DISP_1], DISP_1);
  update_string(display_line[DISP_2], DISP_2);
  update_string(display_line[DISP_3], DISP_3);
  display_changed = BOOLEAN_TRUE;
}

void set_line(const char* line_str, int line_number, int should_clear_if_bad) {
  if ((line_str[0] != '\0') || (line_str[0] != 0))
    strcpy(display_line[line_number], line_str);
  else if (should_clear_if_bad) {
    strcpy(display_line[line_number], EMPTY_STR);
  }
}

void display_screen(const char* l0, const char* l1, const char* l2,
                    const char* l3, int should_clear) {
  if (should_clear) {
    set_clear_lines();
  }
  set_line(l0, DISP_0, BOOLEAN_FALSE);
  set_line(l1, DISP_1, BOOLEAN_FALSE);
  set_line(l2, DISP_2, BOOLEAN_FALSE);
  set_line(l3, DISP_3, BOOLEAN_FALSE);
  update_lines();
}