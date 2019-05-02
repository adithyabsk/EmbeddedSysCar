/** @file main.c
 *  @brief Main embedded system entry point and loop
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include <string.h>

#include "msp430.h"

#include "adc.h"
#include "clocks.h"
#include "commands.h"
#include "common.h"
#include "display.h"
#include "display_text.h"
#include "drive.h"
#include "iot.h"
#include "menu.h"
#include "ports.h"
#include "scheduler.h"
#include "serial.h"
#include "switches.h"
#include "system.h"
#include "timers.h"

void main(void) {
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings

  PM5CTL0 &= ~LOCKLPM5;

  init_ports();              // Initialize Ports
  init_clocks();             // Initialize Clock System
  init_serial();             // Initialize serial ports
  init_display();            // Setup display
  init_timers();             // Initialize Timers
  Init_LCD();                // Initialize LCD
  init_adc();                // Initialize the ADC
  init_scheduler();          // Initialize time based system scheduler
  init_scroll();             // Initialize the scroll of the menu system
  init_iot();                // Initialize iot module and schedule commands
  init_switches();           // Setup switch control variables
  init_drive();              // Initialize car drive state control
  init_drive_config();       // Initialize variable parameters in drive PID
  init_command_processor();  // Initialize commands queue data structure
  init_auto_setup();         // Initialize autonomous setup
  iot_alive();               // Configure IOT status ping
  enable_interrupts();       // Allow interrupts

  while (BOOLEAN_TRUE) {
    menu_state_controller();
    run_scheduler();
    search_cmds();
    process_cmd_queue();
    Display_Process();  // Update display
  }
}
