//------------------------------------------------------------------------------
//
//  Description: This file defines the timer based interrupts
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include  "functions.h"
#include "macros.h"
#include "msp430.h"

extern volatile unsigned char update_display;
extern unsigned int switch_active;

volatile char one_time;
volatile unsigned int Time_Sequence;
unsigned int blink_count;
unsigned int displayer_count;
unsigned int switch_debounce_count;

unsigned int for_rev_count;
volatile char fr_run_status;
volatile unsigned int changed_fr_run_status;

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

#pragma vector = TIMER1_B0_VECTOR
__interrupt void TIMER1_B0_ISR(void) {
  TB1CCR0 += TB1CCR0_INTERVAL;  // Add Offset to TBCCR1
  if (for_rev_count == 0) {
    changed_fr_run_status = 1;
    fr_run_status = FORWARD;
    drive_forward();
    // drive forward one second
  } else if (for_rev_count == 20) {
    // wait one second
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count == 40) {
    // drive backwards two second
    changed_fr_run_status = 1;
    fr_run_status = REVERSE;
    drive_reverse();
  } else if (for_rev_count == 80) {
    // wait one second
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count == 100) {
    // drive forward one second
    changed_fr_run_status = 1;
    fr_run_status = FORWARD;
    drive_forward();
  } else if (for_rev_count == 120) {
    // wait one second
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count == 180) {
    // spin cw 3 seconds
    changed_fr_run_status = 1;
    fr_run_status = CW;
    drive_cw();
  } else if (for_rev_count == 240) {
    // wait two seconds
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count == 300) {
    // spin ccw 3 seconds
    changed_fr_run_status = 1;
    fr_run_status = CCW;
    drive_ccw();
  } else if (for_rev_count == 340) {
    // wait two seconds
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count == 380) {
    // end run
    changed_fr_run_status = 1;
    fr_run_status = WAIT;
    stop_drive();
  } else if (for_rev_count > 380){
    // Turn off interrupt
    TB1CCTL0 &= ~CCIE; // Turn off fr_rev timer
  }
  for_rev_count++;
}