/** @file timers.c
 *  @brief Initialize system timers
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define TIMERS_LOCAL_DEF
#include "timers.h"

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "ports.h"
#include "timers.h"

#define UPDATE_DISPLAY_MAX (4)
#define SWITCH_DEBOUNCE_MAX (24)
#define LINE_FOLLOW_MAX (1750)
#define TURN_MAX (250)
#define WAIT_MAX (250)

// External LCD driver variable
extern volatile unsigned char update_display;

unsigned int displayer_count;
unsigned int for_rev_count;

unsigned int turn_time_count;
unsigned int line_follow_count;
unsigned int wait_time_count;

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

  // Line follow interrupt (10 msec)
  TB1CCR1 = TB1CCR1_INTERVAL;  // CCR1
  TB1CCTL1 &= ~CCIE;           // CCR1 disable interrupt

  // Wall clock
  TB1CCR2 = TB1CCR2_INTERVAL;  // CCR2
  TB1CCTL2 |= CCIE;            // CCR2 enable interrupt

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

  TB3CCR0 = (unsigned int)WHEEL_PERIOD;

  TB3CCTL1 = OUTMOD_7;
  RIGHT_FORWARD_SPEED = WHEEL_OFF;

  TB3CCTL2 = OUTMOD_7;
  LEFT_FORWARD_SPEED = WHEEL_OFF;

  TB3CCTL3 = OUTMOD_7;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;

  TB3CCTL4 = OUTMOD_7;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

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
        SW1_ENABLE_IE;
        SW2_ENABLE_IE;
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
  // TB1CCR0 += TB1CCR0_INTERVAL;  // Add Offset to TBCCR1
}

#pragma vector = TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void) {
  switch (__even_in_range(TB1IV, 14)) {
    case 0:
      break;
    case 2:
      TB1CCR1 += TB1CCR1_INTERVAL;  // Add Offset
      switch (lf_routine_state) {
        case INTERCEPTING:
          drive_forward();
          if (fl_state == SIDEWAYS) lf_routine_state = INTERCEPTED;
          break;
        case INTERCEPTED:
          stop_drive();
          if (wait_time_count++ > WAIT_MAX) lf_routine_state = TURNING;
          break;
        case TURNING:
          drive_ccw();
          if (turn_time_count++ > TURN_MAX) lf_routine_state = TURNED;
          break;
        case TURNED:
          stop_drive();
          lf_routine_state = FOLLOWING_LINE;
          break;
        case FOLLOWING_LINE:
          update_speeds();
          if (line_follow_count++ > LINE_FOLLOW_MAX) {
            lf_routine_state = TURNINTOCIRCLE;
            stop_drive();
            line_follow_count = 0;
            turn_time_count = 0;
          }
          break;
        case TURNINTOCIRCLE:
          drive_ccw();
          if (turn_time_count++ > (unsigned int)(TURN_MAX - 100U)) {
            stop_drive();
            TB1CCTL1 &= ~CCIE;
            lf_routine_state = WAITING;
          }
          break;
        default:
          break;
      }
      break;
    case 4:
      TB1CCR2 += TB1CCR2_INTERVAL;
      wall_clock_time_count++;
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
