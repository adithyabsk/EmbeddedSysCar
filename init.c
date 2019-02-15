//------------------------------------------------------------------------------
//
//  Description: This file initializes the display for the board and other
//.
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

void Init_conditions(void);

extern char display_line[4][11];
extern char *display[4];
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;

void Init_Conditions(void) {
  int i;

  for (i = 0; i < 11; i++) {
    display_line[ZERO][i] = RESET_STATE;
    display_line[ONE][i] = RESET_STATE;
    display_line[TWO][i] = RESET_STATE;
    display_line[THREE][i] = RESET_STATE;
  }
  display_line[ZERO][TEN] = RESET_STATE;
  display_line[ONE][TEN] = RESET_STATE;
  display_line[TWO][TEN] = RESET_STATE;
  display_line[THREE][TEN] = RESET_STATE;

  display[ZERO] = &display_line[ZERO][ZERO];
  display[ONE] = &display_line[ONE][ZERO];
  display[TWO] = &display_line[TWO][ZERO];
  display[THREE] = &display_line[THREE][ZERO];
  update_display = RESET_STATE;
  update_display_count = RESET_COUNT;

  // Interrupts are disabled by default, enable them.
  enable_interrupts();
}