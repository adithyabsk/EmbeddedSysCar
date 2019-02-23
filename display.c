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

volatile extern char active_switch;

volatile extern char fr_run_status;
volatile extern unsigned int changed_fr_run_status;

void set_clear_lines(void) {
  strcpy(display_line[DISPL_0], "          ");
  strcpy(display_line[DISPL_1], "          ");
  strcpy(display_line[DISPL_2], "          ");
  strcpy(display_line[DISPL_3], "          ");
}

void update_lines(void) {
  update_string(display_line[DISPL_0], DISP_0);
  update_string(display_line[DISPL_1], DISP_1);
  update_string(display_line[DISPL_2], DISP_2);
  update_string(display_line[DISPL_3], DISP_3);
}

void clear_display(void) {
  set_clear_lines();
  update_lines();
  display_changed = BOOLEAN_TRUE;
}

void reset_display(void) {
  strcpy(display_line[DISPL_0], "   NCSU   ");
  strcpy(display_line[DISPL_1], " WOLFPACK ");
  strcpy(display_line[DISPL_2], "  ECE306  ");
  strcpy(display_line[DISPL_3], "          ");
  update_lines();
  display_changed = BOOLEAN_TRUE;
}

void show_shapes_menu(char shape) {
  clear_display();
  strcpy(display_line[DISPL_0], "SHAPE MENU");
  switch (shape) {
    case STRAIGHT:
      strcpy(display_line[DISPL_2], "straight  ");
      break;
    case TRIANGLE:
      strcpy(display_line[DISPL_2], "triangle  ");
      break;
    case FIGURE_EIGHT:
      strcpy(display_line[DISPL_2], "eight     ");
      break;
    case FOR_REV:
      strcpy(display_line[DISPL_2], "for/rev   ");
      break;
    default:
      strcpy(display_line[DISPL_2], "circle     ");
  }
  update_lines();
  display_changed = BOOLEAN_TRUE;
}

void show_fr_run_status(void) {
  if (changed_fr_run_status) {
    changed_fr_run_status = 0;

    clear_display();
    strcpy(display_line[DISPL_0], "Run Status");
    switch (fr_run_status) {
      case FORWARD:
        strcpy(display_line[DISPL_3], "forward   ");
        break;
      case REVERSE:
        strcpy(display_line[DISPL_3], "reverse   ");
        break;
      case WAIT:
        strcpy(display_line[DISPL_3], "wait      ");
        break;
      case CW:
        strcpy(display_line[DISPL_3], "cw          ");
        break;
      case CCW:
        strcpy(display_line[DISPL_3], "ccw         ");
        break;
      default:
        break;
    }
    update_lines();
    display_changed = BOOLEAN_TRUE;
  }
}

void show_switch(char sw_status) {
  clear_display();
  strcpy(display_line[DISPL_0], "Selected  ");
  switch (sw_status) {
    case SWITCH_ONE_PRESSED:
      strcpy(display_line[DISPL_3], "Switch 1  ");
      break;
    case SWITCH_TWO_PRESSED:
      strcpy(display_line[DISPL_3], "Switch 2  ");
      break;
    default:
      break;
  }
  update_lines();
  display_changed = BOOLEAN_TRUE;
}

void show_button_status(void) {
  switch (active_switch) {
    case SWITCH_ONE_PRESSED:
      show_switch(SWITCH_ONE_PRESSED);
      break;
    case SWITCH_TWO_PRESSED:
      show_switch(SWITCH_TWO_PRESSED);
      break;
    default:
      break;
  }
}