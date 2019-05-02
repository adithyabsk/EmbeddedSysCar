/** @file switches.c
 *  @brief Implementation for switch interrupt capabilities
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SWITCHES_LOCAL_DEF
#include "switches.h"

#include "msp430.h"

#include "common.h"
#include "ports.h"
#include "timers.h"

inline void init_switches(void) {
  sw1_pressed = INIT_CLEAR;
  sw2_pressed = INIT_CLEAR;
}

#pragma vector = PORT4_VECTOR
__interrupt void switchP4_ISR(void) {
  if (SW1_PRESSED) {
    SW1_CLEAR_IFG;   // Set IGF State
    SW1_DISABLE_IE;  // Disable interrupt
    switch_debounce_count = INIT_CLEAR;
    TB0CCTL2 |= CCIE;  // Turn on debounce timer

    sw1_pressed = BOOLEAN_TRUE;
  }
}

#pragma vector = PORT2_VECTOR
__interrupt void switchP2_ISR(void) {
  if (SW2_PRESSED) {
    SW2_CLEAR_IFG;   // Set IGF State
    SW2_DISABLE_IE;  // Disable interrupt
    switch_debounce_count = INIT_CLEAR;
    TB0CCTL2 |= CCIE;  // Turn on debounce timer

    sw2_pressed = BOOLEAN_TRUE;
  }
}