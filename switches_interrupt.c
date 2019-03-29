/** @file switches_interrupt.c
 *  @brief Implementation for switch interrupt capabilities
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include <string.h>

#define SWITCHES_ISR_LOCAL_DEF
#include "switches_interrupt.h"

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "led.h"
#include "ports.h"
#include "serial.h"

extern unsigned int switch_debounce_count;

extern volatile unsigned int drive_state_holder;
extern volatile unsigned int wall_clock_time_count;

volatile unsigned int baud_rate = 1;
volatile unsigned int switch_press_time = 0;

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

    // if (TB1CCTL1 & CCIE) {  // line follow interrupt is on
    //   TB1CCTL1 &= ~CCIE;
    //   stop_drive();
    //   fl_timer_counter = 1;
    //   prev_fl_state = NO_LINE;
    // } else {
    //   TB1CCTL1 |= CCIE;
    // }

    // toggle baud rates
    if (baud_rate) {
      baud_rate_setup_a0_115200();
      baud_rate_setup_a1_115200();
    } else {
      baud_rate_setup_a0_460800();
      baud_rate_setup_a1_460800();
    }
    baud_rate = !baud_rate;
    switch_press_time = wall_clock_time_count;

    int i;
    for (i = 0; i < 10; i++) {
      usb_char_rx[i] = ' ';
    }
    usb_char_rx[10] = '\0';

    // drive_forward();
  }
}