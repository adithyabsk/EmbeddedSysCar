//------------------------------------------------------------------------------
//
//  Description: This file controls the display
//
//
//  Adithya Balaji
//  Feb 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include <string.h>
#include "functions.h"
#include "macros.h"
#include "msp430.h"

void clear_display(void);

extern char display_line[FOUR][ELEVEN];
extern char *display[FOUR];
extern volatile unsigned char display_changed;

void set_clear_lines(void) {
  strcpy(display_line[DISP_0], "          ");
  strcpy(display_line[DISP_1], "          ");
  strcpy(display_line[DISP_2], "          ");
  strcpy(display_line[DISP_3], "          ");
}

void update_lines(void) {
  update_string(display_line[DISP_0], UPDATE_0);
  update_string(display_line[DISP_1], UPDATE_1);
  update_string(display_line[DISP_2], UPDATE_2);
  update_string(display_line[DISP_3], UPDATE_3);
}

void clear_display(void) {
  set_clear_lines();
  update_lines();
}

void reset_display(void) {
  strcpy(display_line[DISP_0], "   NCSU   ");
  update_string(display_line[DISP_0], UPDATE_0);
  strcpy(display_line[DISP_1], " WOLFPACK ");
  update_string(display_line[DISP_1], UPDATE_1);
  strcpy(display_line[DISP_2], "  ECE306  ");
  update_string(display_line[DISP_2], UPDATE_2);
  strcpy(display_line[DISP_3], "          ");
  update_string(display_line[DISP_3], UPDATE_3);
}

void show_shapes_menu(char shape) {
  clear_display();
  strcpy(display_line[DISP_0], "SHAPE MENU");
  switch (shape) {
    case STRAIGHT:
      strcpy(display_line[DISP_2], "straight  ");
      break;
    case TRIANGLE:
      strcpy(display_line[DISP_2], "triangle  ");
      break;
    case FIGURE_EIGHT:
      strcpy(display_line[DISP_2], "eight     ");
      break;
    default:
      strcpy(display_line[DISP_2], "circle     ");
  }
  update_lines();
}