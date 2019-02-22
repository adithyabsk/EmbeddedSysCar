//------------------------------------------------------------------------------
//
//  Description: This file defines the timer based interrupts
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "macros.h"
#include "msp430.h"

extern volatile unsigned char update_display;
volatile char one_time;
volatile unsigned int Time_Sequence;
unsigned int blink_count;
unsigned int displayer_count;

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
  switch (__even_in_range(TB0IV, 14)) {
    case 0:  // Time_Sequence interrupt
      one_time = 1;
      if (Time_Sequence++ > 250) {
        Time_Sequence = 0;
      }
      TB0CCR0 += TB0CCR0_INTERVAL;  // Add Offset to TBCCR1
      break;
    case 2:  // Display backlite flicker interrupt
      if (blink_count++ > DISPLAY_FLICKER_MAX) {
        blink_count = 0;
        P6OUT ^= LCD_BACKLITE;
      }
      if (displayer_count++ > UPDATE_DISPLAY_MAX) {
        displayer_count = 0;
        update_display = 1;
      }
      TB0CCR1 += TB0CCR1_INTERVAL;  // Add Offset to TBCCR1
      break;
    // case 4: // CCR2 fired
    //   // Interrupt Code
    //   TB0CCR2 += TB0CCR2_INTERVAL; // Add Offset to TBCCR2
    //   break;
    // case 14: // Disabled
    //   // overflow
    //   break;
    default:
      break;
  }
}