//------------------------------------------------------------------------------
//
//  Description: This file controls the interrupt capability of switches
//
//
//  Adithya Balaji
//  Feb 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

extern unsigned int switch_debounce_count;

volatile char active_switch = NONE;

volatile unsigned int should_cycle_shapes = INIT_STATE_ZERO;
volatile unsigned int should_set_run = INIT_STATE_ZERO;

#pragma vector = PORT4_VECTOR
__interrupt void switchP4_ISR(void) {
  if(P4IFG & SW1){
    P4IFG &= ~SW1; // Set IGF State
    P4IE &= ~SW1; // Disable interrupt
    active_switch = SWITCH_ONE_PRESSED;
    switch_debounce_count = INIT_STATE_ZERO;
    TB0CCTL2 |= CCIE; // Turn on debounce timer
    TB0CCTL1 &= ~CCIE; // Turn off flicker timer

    TB1CCTL0 |= CCIE; // Run for_rev interrupt

    P6OUT &= ~LCD_BACKLITE;
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void switchP2_ISR(void) {
  if(P2IFG & SW2){
    P2IFG &= ~SW2; // Set IGF State
    P2IE &= ~SW2; // Disable interrupt
    active_switch = SWITCH_TWO_PRESSED;
    switch_debounce_count = INIT_STATE_ZERO;
    TB0CCTL2 |= CCIE; // Turn on debounce timer
    TB0CCTL1 &= ~CCIE; // Turn off flicker timer

    P6OUT &= ~LCD_BACKLITE;
  }
}