//******************************************************************************
//
//  Description: This file contains the Function prototypes
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (5.40.1)
//  Initial version by Jim Carlson
//******************************************************************************

// General Macros
#define BOOLEAN_TRUE (1)
#define INIT_STATE_ZERO (0)
#define LOW (0x00)
#define OUTPUT (0xFF)

// Port 1 Definitions
#define RED_LED (0x01)     // 0
#define A1_SEEED (0x02)    // 1
#define V_DETECT_L (0x04)  // 2
#define V_DETECT_R (0x08)  // 3
#define A4_SEEED (0x10)    // 4
#define V_THUMB (0x20)     // 5
#define UCA0RXD (0x40)     // 6
#define UCA0TXD (0x80)     // 7

// Port 2 Definitions
#define P2_0 (0x01)    // 0
#define P2_1 (0x02)    // 1
#define P2_2 (0x04)    // 2
#define SW2 (0x08)     // 3
#define P2_4 (0x10)    // 4
#define P2_5 (0x20)    // 5
#define LFXOUT (0x40)  // 6
#define LFXIN (0x80)   // 7

// Port 3 Definitions
#define TEST_PROBE (0x01)  // 0
#define OA20 (0x02)        // 1
#define OA2N (0x04)        // 2
#define OA2P (0x08)        // 3
#define SMCLK_OUT (0x10)   // 4
#define OA3O (0x20)        // 5
#define P3_6 (0x40)        // 6
#define P3_7 (0x80)        // 7

// Port 3.4 Switcher
#define USE_GPIO (0x00)
#define USE_SMCLK (0x01)

// Port 4 Definitions
#define RESET_LCD (0x01)    // 0
#define SW1 (0x02)          // 1
#define UCA1RXD (0x04)      // 2
#define UCA1TXD (0x08)      // 3
#define UCB1_CS_LCD (0x10)  // 4
#define UCB1CLK (0x20)      // 5
#define UCB1SIMO (0x40)     // 6
#define UCB1SOMI (0x80)     // 7

// Port 5 Definitions
#define IOT_RESET (0x01)      // 0
#define IOT_LINK (0x02)       // 1
#define IOT_PROG_SEL (0x04)   // 2
#define IOT_PROG_MODE (0x08)  // 3
#define IR_LED (0x10)         // 4

// Port 6 Definitions
#define R_FORWARD (0x01)     // 0
#define L_FORWARD (0x02)     // 1
#define R_REVERSE (0x04)     // 2
#define L_REVERSE (0x08)     // 3
#define LCD_BACKLITE (0x10)  // 4
#define P6_5 (0x20)          // 5
#define GRN_LED (0x40)       // 6

// clocks.c macros
#define MCLK_FREQ_MHZ (8)  // MCLK = 8MHz
#define CLEAR_REGISTER (0X0000)

// main.c macros
#define RED_LED_ON (P1OUT |= RED_LED)
#define RED_LED_OFF (P1OUT &= ~RED_LED)
#define GREEN_LED_ON (P6OUT |= GRN_LED)
#define GREEN_LED_OFF (P6OUT &= ~GRN_LED)

// Indicies
#define ZERO (0)
#define ONE (1)
#define TWO (2)
#define THREE (3)
#define FOUR (4)
#define FIVE (5)
#define SIX (6)
#define TEN (10)
#define ELEVEN (11)

// Drive
#define R_DRIVE_ON (P6OUT |= R_FORWARD)
#define L_DRIVE_ON (P6OUT |= L_FORWARD)
#define R_DRIVE_OFF (P6OUT &= ~R_FORWARD)
#define L_DRIVE_OFF (P6OUT &= ~L_FORWARD)

#define R_REVERSE_ON (P6OUT |= R_REVERSE)
#define L_REVERSE_ON (P6OUT |= L_REVERSE)
#define R_REVERSE_OFF (P6OUT &= ~R_REVERSE)
#define L_REVERSE_OFF (P6OUT &= ~L_REVERSE)

// Change clocks
#define MCLK4MHz (CSCTL5 |= DIVM__2)
#define SMCLK500kHz (CSCTL5 |= DIVS__8)
#define MCLKReset (CSCTL5 |= DIVM__1)
#define SMCLKReset (CSCTL5 |= DIVS__1)

// Switches
#define DEBOUNCE_DELAY (1)

// Shapes
#define NONE ('N')
#define STRAIGHT ('S')
#define STRAIGHT_ENUM (1)
#define FIGURE_EIGHT ('E')
#define FIGURE_EIGHT_ENUM (2)
#define LOOP_2 (2)
#define LOOP_3 (3)
#define TRIANGLE ('T')
#define TRIANGLE_ENUM (3)
#define FOR_REV ('R')
#define FOR_REV_ENUM (4)
#define CIRCLE ('C')
#define CIRCLE_ENUM (0)
#define SHAPE_COUNT (5)

#define WAIT ('W')
#define START ('S')
#define RUN ('R')
#define END ('E')

// forward rev state
#define FORWARD ('a')
#define REVERSE ('b')
// WAIT already defined
#define CW ('d')
#define CCW ('e')

#define RUN_TRUE (1)
#define WHEEL_COUNT_TIME (10)
#define LEFT_COUNT_TIME (2)
#define RIGHT_COUNT_TIME (9)
#define TRAVEL_DISTANCE (100)
#define WAITING2START (50)

// Display macros
#define DISPL_0 (0)
#define DISPL_1 (1)
#define DISPL_2 (2)
#define DISPL_3 (3)
#define DISPL_START (0)
#define DISP_0 (0)
#define DISP_1 (1)
#define DISP_2 (2)
#define DISP_3 (3)

// Main macros
#define DELAY_TIME (1000000)
#define TIME_250 (250)
#define TIME_200 (200)
#define TIME_150 (150)
#define TIME_100 (100)
#define TIME_50 (50)

#define DISPLAY_FLICKER_MAX (10)
#define UPDATE_DISPLAY_MAX (4)
#define SWITCH_DEBOUNCE_MAX (24)

// Timer B0
// Calculation SMCLK / 2 / 8 / (1/x in seconds) --> rate
#define TB0CCR0_INTERVAL (25000)   // 8,000,000/2/8/[1/50msec] --> 50msec
#define TB0CCR1_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50msec
#define TB0CCR2_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50msec

// Timer B1
#define TB1CCR0_INTERVAL (25000)  // 8,000,000/2/8/[1/50msec] --> 50msec

// Main While loop switch cases
#define Time_Sequence_Rate (50) // 50 millisecods
#define S1250 (1250/Time_Sequence_Rate) // 1.25 seconds
#define S1000 (1000/Time_Sequence_Rate) // 1 second
#define S750 (750/Time_Sequence_Rate) // 0.75 seconds
#define S500 (500/Time_Sequence_Rate) // 0.50 seconds
#define S250 (250/Time_Sequence_Rate) // 0.25 seconds


#define SWITCH_ONE_PRESSED ('A')
#define SWITCH_TWO_PRESSED ('B')

