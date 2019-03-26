/** @file switches_interrupt.c
 *  @brief Implementation for switch interrupt capabilities
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SWITCHES_ISR_LOCAL_DEF
#include "switches_interrupt.h"

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "led.h"
#include "ports.h"

extern unsigned int switch_debounce_count;

volatile extern unsigned int drive_state_holder;

#pragma vector = PORT4_VECTOR
__interrupt void switchP4_ISR(void) {
  if (P4IFG & SW1) {
    P4IFG &= ~SW1;  // Set IGF State
    P4IE &= ~SW1;   // Disable interrupt
    switch_debounce_count = INIT_CLEAR;
    TB0CCTL2 |= CCIE;  // Turn on debounce timer

    // Toggle on emitter
    IR_LED_TOGGLE;
    ir_status = !ir_status;
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void switchP2_ISR(void) {
  if (P2IFG & SW2) {
    P2IFG &= ~SW2;  // Set IGF State
    P2IE &= ~SW2;   // Disable interrupt
    switch_debounce_count = INIT_CLEAR;
    TB0CCTL2 |= CCIE;  // Turn on debounce timer

    // TB1CCTL0 |= CCIE;  // Run for_rev interrupt (runs forward reverse
    // process)

    if (TB1CCTL1 & CCIE) {  // line follow interrupt is on
      TB1CCTL1 &= ~CCIE;
      stop_drive();
      fl_timer_counter = 1;
      prev_fl_state = NO_LINE;
    } else {
      TB1CCTL1 |= CCIE;
    }

    // drive_forward();
  }
}