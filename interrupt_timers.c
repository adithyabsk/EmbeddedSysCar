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
extern unsigned int switch_active;

volatile char one_time;
volatile unsigned int Time_Sequence;
unsigned int blink_count;
unsigned int displayer_count;
unsigned int switch_debounce_count;

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMER0_B0_ISR(void) {
  one_time = 1;
  if (Time_Sequence++ > 250) {
    Time_Sequence = 0;
  }
  if (displayer_count++ > UPDATE_DISPLAY_MAX) {
    displayer_count = 0;
    update_display = 1;
  }
  TB0CCR0 += TB0CCR0_INTERVAL;  // Add Offset to TBCCR1
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void) {
  switch (__even_in_range(TB0IV, 14)) {
    case 0:  // just break
      break;
    case 2:  // Display backlite flicker interrupt
      TB0CCR1 += TB0CCR1_INTERVAL;  // Add Offset to TBCCR1
      if (blink_count++ > DISPLAY_FLICKER_MAX) {
        blink_count = INIT_STATE_ZERO;
        P6OUT ^= LCD_BACKLITE;
      }
      break;
    case 4: // CCR2 fired
      TB0CCR2 += TB0CCR2_INTERVAL; // Add Offset to TBCCR2
      if (switch_debounce_count++ > SWITCH_DEBOUNCE_MAX) {
        switch_debounce_count = INIT_STATE_ZERO;
        P4IE |= SW1;
        P2IE |= SW2;
        TB0CCTL1 |= CCIE; // Turn on flicker timer
        TB0CCTL2 &= ~CCIE; // Turn off debounce timer
      }
      break;
    // case 14: // Disabled
    //   // overflow
    //   break;
    default:
      break;
  }
}