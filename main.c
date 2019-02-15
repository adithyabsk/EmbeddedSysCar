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
    // set_drive();
    __delay_cycles(DELAY_TIME);             // Dely LCD
    // Place the contents of what you want on the display, in between the quotes
    // Limited to 10 characters per line

    strcpy(display_line[DISP_0], "   NCSU   ");
    update_string(display_line[DISP_0], UPDATE_0);
    strcpy(display_line[DISP_1], " WOLFPACK ");
    update_string(display_line[DISP_1], UPDATE_1);
    strcpy(display_line[DISP_2], "  ECE306  ");
    update_string(display_line[DISP_3], UPDATE_3);
    enable_display_update();
    //  Display_Update(3,1,0,0);

    //------------------------------------------------------------------------------
    // Begining of the "While" Operating System
    //------------------------------------------------------------------------------
    while (ALWAYS) {                     // Can the Operating system run
        switch (Time_Sequence) {
        case TIME_250:
            if (one_time) {
                Init_LEDs();
                lcd_BIG_mid();
                display_changed = DISP_CHANGED;
                one_time = INIT;
            }
            Time_Sequence = INIT;
            break;
        case TIME_200:
            if (one_time) {
                GREEN_LED_ON;            // Change State of LED 5
                one_time = INIT;
            }
            break;
        case TIME_150:
            if (one_time) {
                RED_LED_ON;              // Change State of LED 4
                GREEN_LED_OFF;           // Change State of LED 5
                one_time = INIT;
            }
            break;
        case TIME_100:
            if (one_time) {
                lcd_4line();
                GREEN_LED_ON;            // Change State of LED 5
                display_changed = DISP_CHANGED;
                one_time = INIT;
            }
            break;
        case  TIME_50:
            if (one_time) {
                RED_LED_OFF;             // Change State of LED 4
                GREEN_LED_OFF;           // Change State of LED 5
                one_time = INIT;
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
