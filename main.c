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
#include "display.h"
#include "display_text.h"
#include "drive.h"
#include "iot.h"
#include "led.h"
#include "menu.h"

#include "ports.h"
#include "scheduler.h"
#include "serial.h"
#include "switches.h"
#include "system.h"
#include "timers.h"

#include "commands.h"

#define DELAY_TIME (1000000)

// Global Variables
// volatile char slow_input_down;

void main(void) {
  // Disable the GPIO power-on default high-impedance mode to activate
  // previously configured port settings

  PM5CTL0 &= ~LOCKLPM5;

  fl_timer_counter = 1;

  init_ports();         // Initialize Ports
  init_clocks();        // Initialize Clock System
  init_serial();        // Initialize serial ports
  enable_interrupts();  // Allow interrupts
  init_display();       // Setup display
  init_timers();        // Initialize Timers
  Init_LCD();           // Initialize LCD
  init_adc();           // Initialize the ADC
  init_scheduler();     // Initialize time based system scheduler

  init_scroll();  // Initialize the scroll of the menu system

  init_iot();  // FIX ME
  init_switches();
  init_drive();

  init_command_processor();  // Initialize commands queue data structure

  // iot_alive(); FIX THIS!!

  while (BOOLEAN_TRUE) {  // Operational loop
    process_leds();

    menu_state_controller();

    run_scheduler();
    arcade_drive_state_machine();

    search_cmds();
    process_cmd_queue();

    Display_Process();  // Dispaly update
  }
}
