/** @file timers_interrupt.c
 *  @brief Implementation for timer based interrupts
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define TIMERS_ISR_LOCAL_DEF
#include "timers_interrupt.h"

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "ports.h"
#include "timers.h"

#define UPDATE_DISPLAY_MAX (4)
#define SWITCH_DEBOUNCE_MAX (24)

// External LCD driver variable
extern volatile unsigned char update_display;

unsigned int displayer_count;
unsigned int for_rev_count;

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
      // TB0CCR1 += TB0CCR1_INTERVAL;  // Add Offset to TBCCR1
      break;
    case 4:                         // CCR2 fired
      TB0CCR2 += TB0CCR2_INTERVAL;  // Add Offset to TBCCR2
      if (switch_debounce_count++ > SWITCH_DEBOUNCE_MAX) {
        switch_debounce_count = INIT_CLEAR;
        P4IE |= SW1;
        P2IE |= SW2;
        TB0CCTL1 |= CCIE;   // Turn on flicker timer
        TB0CCTL2 &= ~CCIE;  // Turn off debounce timer
      }
      break;
    case 14:  // Disabled
      // overflow
      break;
    default:
      break;
  }
}

#pragma vector = TIMER1_B0_VECTOR
__interrupt void TIMER1_B0_ISR(void) {
  TB1CCR0 += TB1CCR0_INTERVAL;  // Add Offset to TBCCR1
  if (for_rev_count == 0) {
    drive_forward();
    // drive forward one second
  } else if (for_rev_count == 20) {
    // wait one second
    stop_drive();
  } else if (for_rev_count == 40) {
    // drive backwards two second
    drive_reverse();
  } else if (for_rev_count == 80) {
    // wait one second
    stop_drive();
  } else if (for_rev_count == 100) {
    // drive forward one second
    drive_forward();
  } else if (for_rev_count == 120) {
    // wait one second
    stop_drive();
  } else if (for_rev_count == 180) {
    // spin cw 3 seconds
    drive_cw();
  } else if (for_rev_count == 240) {
    // wait two seconds
    stop_drive();
  } else if (for_rev_count == 300) {
    // spin ccw 3 seconds
    drive_ccw();
  } else if (for_rev_count == 340) {
    // wait two seconds
    stop_drive();
  } else if (for_rev_count == 380) {
    // end run
    stop_drive();
  } else if (for_rev_count > 380) {
    // Turn off interrupt
    TB1CCTL0 &= ~CCIE;  // Turn off fr_rev timer
  }
  for_rev_count++;
}

#pragma vector = TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void) {
  switch (__even_in_range(TB1IV, 14)) {
    case 0:
      break;
    case 2:
      // TB1CCR1 += TB1CCR1_INTERVAL;  // Add Offset
      break;
    case 4:
      // TB1CCR2 += TB1CCR2_INTERVAL;  // Add Offset
      break;
    case 14:
      // overflow
      break;
    default:
      break;
  }
}

#pragma vector = TIMER2_B0_VECTOR
__interrupt void TIMER2_B0_ISR(void) {
  // TB2CCR0 += TB2CCR0_INTERVAL;  // Add Offset
}

#pragma vector = TIMER2_B1_VECTOR
__interrupt void TIMER2_B1_ISR(void) {
  switch (__even_in_range(TB2IV, 14)) {
    case 0:
      break;
    case 2:
      // TB2CCR1 += TB2CCR1_INTERVAL;  // Add Offset
      break;
    case 4:
      // TB2CCR2 += TB2CCR2_INTERVAL;  // Add Offset
      break;
    case 14:
      // overflow
      break;
    default:
      break;
  }
}

#pragma vector = TIMER3_B0_VECTOR
__interrupt void TIMER3_B0_ISR(void) {
  // TB3CCR0 += TB3CCR0_INTERVAL;  // Add Offset
}

#pragma vector = TIMER3_B1_VECTOR
__interrupt void TIMER3_B1_ISR(void) {
  switch (__even_in_range(TB3IV, 14)) {
    case 0:
      break;
    case 2:
      // TB3CCR1 += TB3CCR1_INTERVAL;  // Add Offset
      break;
    case 4:
      // TB3CCR2 += TB3CCR2_INTERVAL;  // Add Offset
      break;
    case 14:
      // overflow
      break;
    default:
      break;
  }
}