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
#include "ports.h"
#include "system.h"
#include "timers.h"

#define DELAY_TIME (1000000)

// Function Prototypes
void main(void);

// Global Variables
// volatile char slow_input_down;

void main(void) {
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;
  init_ports();                // Initialize Ports
  init_clocks();               // Initialize Clock System
  enable_interrupts();         // Allow interrupts
  init_display();              // Setup display
  init_timers();               // Initialize Timers
  Init_LCD();                  // Initialize LCD
  init_adc();                  // Initialize the ADC
  __delay_cycles(DELAY_TIME);  // Dely LCD
  reset_display();             // Set default display

  while (BOOLEAN_TRUE) {  // Operational loop
    process_leds();
    show_adc_status();
    Display_Process();  // Dispaly update
  }
}
