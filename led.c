/** @file led.c
 *  @brief Implementation for led initialization
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "led.h"

#include "msp430.h"

#include "common.h"
#include "ports.h"
#include "timers_interrupt.h"

#define Time_Sequence_Rate (50)            // 50 millisecods
#define S1250 (1250 / Time_Sequence_Rate)  // 1.25 seconds
#define S1000 (1000 / Time_Sequence_Rate)  // 1 second
#define S750 (750 / Time_Sequence_Rate)    // 0.75 seconds
#define S500 (500 / Time_Sequence_Rate)    // 0.50 seconds
#define S250 (250 / Time_Sequence_Rate)    // 0.25 seconds

#define RED_LED_ON (P1OUT |= RED_LED)
#define RED_LED_OFF (P1OUT &= ~RED_LED)
#define GREEN_LED_ON (P6OUT |= GRN_LED)
#define GREEN_LED_OFF (P6OUT &= ~GRN_LED)

void process_leds(void) {
  switch (Time_Sequence) {
    case S1250:
      if (one_time) {
        GREEN_LED_OFF;
        RED_LED_OFF;
        one_time = INIT_CLEAR;
      }
      Time_Sequence = INIT_CLEAR;
      break;
    case S1000:
      if (one_time) {
        GREEN_LED_ON;  // Change State of LED 5
        one_time = INIT_CLEAR;
      }
      break;
    case S750:
      if (one_time) {
        RED_LED_ON;     // Change State of LED 4
        GREEN_LED_OFF;  // Change State of LED 5
        one_time = INIT_CLEAR;
      }
      break;
    case S500:
      if (one_time) {
        GREEN_LED_ON;  // Change State of LED 5
        one_time = INIT_CLEAR;
      }
      break;
    case S250:
      if (one_time) {
        RED_LED_OFF;    // Change State of LED 4
        GREEN_LED_OFF;  // Change State of LED 5
        one_time = INIT_CLEAR;
      }
      break;
    default:
      break;
  }
}