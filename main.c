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
#include "macros.h"
#include  "functions.h"
#include  "msp430.h"
#include <string.h>


// Function Prototypes
void main(void);
void set_drive(void);

// Global Variables
volatile char slow_input_down;
extern char display_line[4][11];
extern char *display[4];
unsigned char display_mode;
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
unsigned int test_value;
char chosen_direction;
char change;

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
    Init_Ports();                        // Initialize Ports
    Init_Clocks();                       // Initialize Clock System
    Init_Conditions();                   // Initialize Variables and Initial Conditions
    Init_Timers();                       // Initialize Timers
    Init_LCD();                          // Initialize LCD
    set_drive();
    __delay_cycles(1000000);             // Dely LCD
    // Place the contents of what you want on the display, in between the quotes
    // Limited to 10 characters per line

    strcpy(display_line[0], "   NCSU   ");
    update_string(display_line[0], 0);
    strcpy(display_line[1], " WOLFPACK ");
    update_string(display_line[1], 1);
    strcpy(display_line[2], "  ECE306  ");
    update_string(display_line[3], 3);
    enable_display_update();
    //  Display_Update(3,1,0,0);

    //------------------------------------------------------------------------------
    // Begining of the "While" Operating System
    //------------------------------------------------------------------------------
    while (ALWAYS) {                     // Can the Operating system run
        switch (Time_Sequence) {
        case 250:
            if (one_time) {
                Init_LEDs();
                lcd_BIG_mid();
                display_changed = 1;
                one_time = 0;
            }
            Time_Sequence = 0;
            break;
        case 200:
            if (one_time) {
                GREEN_LED_ON;            // Change State of LED 5
                one_time = 0;
            }
            break;
        case 150:
            if (one_time) {
                RED_LED_ON;              // Change State of LED 4
                GREEN_LED_OFF;           // Change State of LED 5
                one_time = 0;
            }
            break;
        case 100:
            if (one_time) {
                lcd_4line();
                GREEN_LED_ON;            // Change State of LED 5
                display_changed = 1;
                one_time = 0;
            }
            break;
        case  50:
            if (one_time) {
                RED_LED_OFF;             // Change State of LED 4
                GREEN_LED_OFF;           // Change State of LED 5
                one_time = 0;
            }
            break;
        default: break;
        }
        Switches_Process();              // Check for switch state change
        Display_Process();
        //    Wheels();

    }
}

void set_drive(void) {
    R_DRIVE;
    L_DRIVE;
}
