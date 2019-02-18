//------------------------------------------------------------------------------
//
//  Description: Main operating loop
//
//
//  Adithya Balaji
//  Jan 2018
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.11.2)
//  Initial version by Jim Carlson
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include <string.h>
#include "functions.h"
#include "macros.h"
#include "msp430.h"

// Function Prototypes
void main(void);

// Global Variables
volatile char slow_input_down;
extern char display_line[FOUR][ELEVEN];
extern char *display[FOUR];
unsigned char display_mode;
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
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
  Init_Ports();       // Initialize Ports
  Init_Clocks();      // Initialize Clock System
  Init_Conditions();  // Initialize Variables and Initial Conditions
  Init_Timers();      // Initialize Timers
  Init_LCD();         // Initialize LCD
  set_switch_states();
  default_shape_setup();
  __delay_cycles(DELAY_TIME);  // Dely LCD
  // Place the contents of what you want on the display, in between the quotes
  // Limited to 10 characters per line

  reset_display();
  enable_display_update();
  display_changed = DISP_CHANGED;
  //  Display_Update(3,1,0,0);

  Last_Time_Sequence = INIT;
  cycle_time = INIT;
  time_change = INIT;
  normal_time = INIT;

  //------------------------------------------------------------------------------
  // Begining of the "While" Operating System
  //------------------------------------------------------------------------------
  while (ALWAYS) {  // Can the Operating system run
    if (Last_Time_Sequence != Time_Sequence) {
      Last_Time_Sequence = Time_Sequence;
      cycle_time++;
      normal_time++;
      time_change = 1;
    }
    process_shapes();

    switch (Time_Sequence) {
      case TIME_250:
        if (one_time) {
          Init_LEDs();
          // lcd_BIG_mid();
          // clear_display();
          display_changed = DISP_CHANGED;

          one_time = INIT;
        }
        Time_Sequence = INIT;
        break;
      case TIME_200:
        if (one_time) {
          GREEN_LED_ON;  // Change State of LED 5
          one_time = INIT;
        }
        break;
      case TIME_150:
        if (one_time) {
          RED_LED_ON;     // Change State of LED 4
          GREEN_LED_OFF;  // Change State of LED 5
          one_time = INIT;
        }
        break;
      case TIME_100:
        if (one_time) {
          // lcd_4line();
          GREEN_LED_ON;  // Change State of LED 5
          // show_shapes_menu(CIRCLE);
          display_changed = DISP_CHANGED;
          one_time = INIT;
        }
        break;
      case TIME_50:
        if (one_time) {
          RED_LED_OFF;    // Change State of LED 4
          GREEN_LED_OFF;  // Change State of LED 5
          one_time = INIT;
        }
        break;
      default:
        break;
    }
    Display_Process();
    Switches_Process();  // Check for switch state change
    //    Wheels();
  }
}
