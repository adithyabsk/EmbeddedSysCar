/** @file timers.c
 *  @brief Initialize system timers
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "timers.h"

#include "msp430.h"

#include "common.h"
#include "drive.h"

void init_timer_B0(void);
void init_timer_B1(void);
void init_timer_B2(void);
void init_timer_B3(void);

void init_timers(void) {
  init_timer_B0();
  init_timer_B1();
  init_timer_B2();
  init_timer_B3();
}

void init_timer_B0(void) {
  TB0CTL = INIT_CLEAR;
  TB0EX0 = INIT_CLEAR;
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

void init_timer_B1(void) {
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

void init_timer_B2(void) {
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

void init_timer_B3(void) {
  TB3CTL = TBSSEL__SMCLK;  // SMCLK source
  // TB3CTL |= TBCLR;          // Resets TB0R, clock divider, count direction
  // TB3CTL |= MC__CONTINOUS;  // Continuous up
  // TB3CTL |= ID__2;          // Divide clock by 2
  // TB3EX0 = TBIDEX__8;       // Divide clock by an additional 8

  TB3CTL |= MC__UP;  // Up Mode
  TB3CTL |= TBCLR;   // Clear TAR

  TB3CCR0 = WHEEL_PERIOD;

  TB3CCTL1 = OUTMOD_7;
  RIGHT_FORWARD_SPEED = WHEEL_OFF;

  TB3CCTL2 = OUTMOD_7;
  LEFT_FORWARD_SPEED = WHEEL_OFF;

  TB3CCTL3 = OUTMOD_7;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;

  TB3CCTL4 = OUTMOD_7;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}
