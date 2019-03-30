/** @file switches.c
 *  @brief Implementation for switch interrupt capabilities
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SWITCHES_LOCAL_DEF
#include "switches.h"

#include <string.h>

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "led.h"
#include "ports.h"
#include "serial.h"
#include "timers.h"

extern unsigned int switch_debounce_count;

extern volatile unsigned int drive_state_holder;

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
    if (baud_mode) {
      baud_rate_setup_a0_115200();
      baud_rate_setup_a1_115200();
    } else {
      baud_rate_setup_a0_460800();
      baud_rate_setup_a1_460800();
    }
    baud_mode = !baud_mode;
    switch_press_time = wall_clock_time_count;

    clear_ring_buffer((char*)usb_char_rx);
    schedule_transmit();  // schedule a transmission

    // drive_forward();
  }
}