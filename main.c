/** @file main.c
 *  @brief Main embedded system entry point and loop
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include <string.h>

#include "msp430.h"

#include "adc.h"
#include "clocks.h"
#include "common.h"
#include "display_text.h"
#include "led.h"
#include "system.h"
#include "timers.h"
#include "timers_interrupt.h"

#define DELAY_TIME (1000000)

#define Time_Sequence_Rate (50)            // 50 millisecods
#define S1250 (1250 / Time_Sequence_Rate)  // 1.25 seconds
#define S1000 (1000 / Time_Sequence_Rate)  // 1 second
#define S750 (750 / Time_Sequence_Rate)    // 0.75 seconds
#define S500 (500 / Time_Sequence_Rate)    // 0.50 seconds
#define S250 (250 / Time_Sequence_Rate)    // 0.25 seconds

// Function Prototypes
void main(void);

// Global Variables
volatile char slow_input_down;
unsigned int test_value;
char chosen_direction;
char change;

unsigned int Last_Time_Sequence;
unsigned int cycle_time;
unsigned int normal_time;
unsigned int time_change;

void main(void) {
  //------------------------------------------------------------------------------
  // Main Program
  // This is the main routine for the program. Execution of code starts here.
  // The operating system is Back Ground Fore Ground.
  //
  //------------------------------------------------------------------------------
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;
  init_ports();   // Initialize Ports
  init_clocks();  // Initialize Clock System
  enable_interrupts();
  init_display();
  init_timers();  // Initialize Timers
  Init_LCD();     // Initialize LCD
  init_adc();     // Initialize the ADC
  // default_shape_setup();
  __delay_cycles(DELAY_TIME);  // Dely LCD
  // Place the contents of what you want on the display, in between the quotes
  // Limited to 10 characters per line

  reset_display();
  // enable_display_update();
  //  Display_Update(3,1,0,0);

  Last_Time_Sequence = INIT_CLEAR;
  cycle_time = INIT_CLEAR;
  time_change = INIT_CLEAR;
  normal_time = INIT_CLEAR;

  //------------------------------------------------------------------------------
  // Begining of the "While" Operating System
  //------------------------------------------------------------------------------
  while (BOOLEAN_TRUE) {  // Can the Operating system run
    if (Last_Time_Sequence != Time_Sequence) {
      Last_Time_Sequence = Time_Sequence;
      cycle_time++;
      normal_time++;
      time_change = 1;
    }

    switch (Time_Sequence) {
      case S1250:
        if (one_time) {
          init_leds();
          // lcd_BIG_mid();
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
          // lcd_4line();
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
    // show_fr_run_status();
    show_adc_status();
    Display_Process();
    //    Wheels();
  }
}
