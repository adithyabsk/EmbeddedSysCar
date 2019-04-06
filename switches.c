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

#pragma vector = PORT4_VECTOR
__interrupt void switchP4_ISR(void) {
  if (SW1_PRESSED) {
    SW1_CLEAR_IFG;   // Set IGF State
    SW1_DISABLE_IE;  // Disable interrupt
    switch_debounce_count = INIT_CLEAR;
    TB0CCTL2 |= CCIE;  // Turn on debounce timer

    sw1_pressed = BOOLEAN_TRUE;

    // Toggle on emitter
    // IR_LED_TOGGLE;
    // ir_status = !ir_status;
    // iot_transmit();
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

    // cycle baud rates

    // if (system_baud < BAUD_MAX) {
    //   system_baud++;
    // } else {
    //   system_baud = BAUD_MIN;
    // }
    // set_iot_baud_rate(system_baud);
    // set_usb_baud_rate(system_baud);
    // switch_press_time = wall_clock_time_count;

    // clear_usb_state();
    // clear_iot_state();
    //
    // schedule_test_transmit();  // schedule a transmission

    // drive_forward();
  }
}