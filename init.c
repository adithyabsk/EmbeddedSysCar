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
    display_line[DISPL_0][i] = INIT_STATE_ZERO;
    display_line[DISPL_1][i] = INIT_STATE_ZERO;
    display_line[DISPL_2][i] = INIT_STATE_ZERO;
    display_line[DISPL_3][i] = INIT_STATE_ZERO;
  }
  display[DISP_0] = &display_line[DISPL_0][DISPL_START];
  display[DISP_1] = &display_line[DISPL_1][DISPL_START];
  display[DISP_2] = &display_line[DISPL_2][DISPL_START];
  display[DISP_3] = &display_line[DISPL_3][DISPL_START];

  update_display = INIT_STATE_ZERO;
  update_display_count = INIT_STATE_ZERO;

  // Interrupts are disabled by default, enable them.
  enable_interrupts();
}