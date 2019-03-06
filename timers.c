//------------------------------------------------------------------------------
//
//  Description: This file initializes the system timers
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

void Init_Timers(void) {
  Init_Timer_B0();
  Init_Timer_B1();
  Init_Timer_B2();
  Init_Timer_B3();
}

void Init_Timer_B0(void) {
  TB0CTL = INIT_STATE_ZERO;
  TB0EX0 = INIT_STATE_ZERO;
  TB0CTL = TBSSEL__SMCLK;   // SMCLK source
  TB0CTL |= MC__CONTINOUS;  // Continuous up
  TB0CTL |= ID__2;          // Divide clock by 2
  TB0EX0 = TBIDEX__8;       // Divide clock by an additional 8
  TB0CTL |= TBCLR;          // Resets TB0R, clock divider, count direction

  TB0CCR0 = TB0CCR0_INTERVAL;  // CCR0
  TB0CCTL0 |= CCIE;            // CCR0 enable interrupt
  // TB0CCR1 = TB0CCR1_INTERVAL;  // CCR1
  // TB0CCTL1 |= CCIE;            // CCR1 enable interrupt
  TB0CCR2 = TB0CCR2_INTERVAL;  // CCR2
  TB0CCTL2 &= ~CCIE;           // CCR2 enable interrupt

  TB0CTL &= ~TBIE;   // Disable Overflow Interrupt
  TB0CTL &= ~TBIFG;  // Clear overflow interrupt flag
}

void Init_Timer_B1(void) {
  TB1CTL = TBSSEL__SMCLK;   // SMCLK source
  TB1CTL |= TBCLR;          // Resets TB0R, clock divider, count direction
  TB1CTL |= MC__CONTINOUS;  // Continuous up
  TB1CTL |= ID__8;          // Divide clock by 8
  TB1EX0 = TBIDEX__8;       // Divide clock by an additional 8

  TB1CCR0 = TB1CCR0_INTERVAL;  // CCR0
  TB1CCTL0 &= ~CCIE;           // CCR0 disable by default interrupt
  // TB1CCR1 = TB1CCR1_INTERVAL;  // CCR1
  // TB1CCTL1 |= CCIE;            // CCR1 enable interrupt
  // TB1CCR2 = TB1CCR2_INTERVAL;  // CCR2
  // TB1CCTL2 |= CCIE;            // CCR2 enable interrupt

  TB1CTL &= ~TBIE;   // Disable Overflow Interrupt
  TB1CTL &= ~TBIFG;  // Clear overflow interrupt flag
}

void Init_Timer_B2(void) {
  TB2CTL = TBSSEL__SMCLK;   // SMCLK source
  TB2CTL |= TBCLR;          // Resets TB0R, clock divider, count direction
  TB2CTL |= MC__CONTINOUS;  // Continuous up
  TB2CTL |= ID__2;          // Divide clock by 2
  TB2EX0 = TBIDEX__8;       // Divide clock by an additional 8

  // TB2CCR0 = TB2CCR0_INTERVAL;    // CCR0
  // TB2CCTL0 |= CCIE;              // CCR0 enable interrupt
  // TB2CCR1 = TB2CCR1_INTERVAL;    // CCR1
  // TB2CCTL1 |= CCIE;              // CCR1 enable interrupt
  // TB2CCR2 = TB2CCR2_INTERVAL;    // CCR2
  // TB2CCTL2 |= CCIE;              // CCR2 enable interrupt

  TB2CTL &= ~TBIE;   // Disable Overflow Interrupt
  TB2CTL &= ~TBIFG;  // Clear overflow interrupt flag
}

void Init_Timer_B3(void) {
  TB3CTL = TBSSEL__SMCLK;   // SMCLK source
  TB3CTL |= TBCLR;          // Resets TB0R, clock divider, count direction
  TB3CTL |= MC__CONTINOUS;  // Continuous up
  TB3CTL |= ID__2;          // Divide clock by 2
  TB3EX0 = TBIDEX__8;       // Divide clock by an additional 8

  // TB3CCR0 = TB3CCR0_INTERVAL;    // CCR0
  // TB3CCTL0 |= CCIE;              // CCR0 enable interrupt
  // TB3CCR1 = TB3CCR1_INTERVAL;    // CCR1
  // TB3CCTL1 |= CCIE;              // CCR1 enable interrupt
  // TB3CCR2 = TB3CCR2_INTERVAL;    // CCR2
  // TB3CCTL2 |= CCIE;              // CCR2 enable interrupt

  TB3CTL &= ~TBIE;   // Disable Overflow Interrupt
  TB3CTL &= ~TBIFG;  // Clear overflow interrupt flag
}
