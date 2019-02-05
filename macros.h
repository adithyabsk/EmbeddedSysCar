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
#define ALWAYS                         (1)
#define RESET_STATE                    (0)
#define RESET_COUNT                    (0)
#define LOW                            (0x00)
#define INIT                           (0x00)
#define OUTPUT                         (0xFF)

// Port 1 Definitions
#define RED_LED                        (0x01) // 0
#define A1_SEEED                       (0x02) // 1
#define V_DETECT_L                     (0x04) // 2
#define V_DETECT_R                     (0x08) // 3
#define A4_SEEED                       (0x10) // 4
#define V_THUMB                        (0x20) // 5
#define UCA0RXD                        (0x40) // 6
#define UCA0TXD                        (0x80) // 7

// Port 2 Definitions
#define P2_0                           (0x01) // 0
#define P2_1                           (0x02) // 1
#define P2_2                           (0x04) // 2
#define SW2                            (0x08) // 3
#define P2_4                           (0x10) // 4
#define P2_5                           (0x20) // 5
#define LFXOUT                         (0x40) // 6
#define LFXIN                          (0x80) // 7

// Port 3 Definitions
#define TEST_PROBE                     (0x01) // 0
#define OA20                           (0x02) // 1
#define OA2N                           (0x04) // 2
#define OA2P                           (0x08) // 3
#define SMCLK_OUT                      (0x10) // 4
#define OA3O                           (0x20) // 5
#define P3_6                           (0x40) // 6
#define P3_7                           (0x80) // 7

// Port 4 Definitions
#define RESET_LCD                      (0x01) // 0
#define SW1                            (0x02) // 1
#define UCA1RXD                        (0x04) // 2
#define UCA1TXD                        (0x08) // 3
#define UCB1_CS_LCD                    (0x10) // 4
#define UCB1CLK                        (0x20) // 5
#define UCB1SIMO                       (0x40) // 6
#define UCB1SOMI                       (0x80) // 7

// Port 5 Definitions
#define IOT_RESET                      (0x01) // 0
#define IOT_LINK                       (0x02) // 1
#define IOT_PROG_SEL                   (0x04) // 2
#define IOT_PROG_MODE                  (0x08) // 3
#define IR_LED                         (0x10) // 4

// Port 6 Definitions
#define R_FORWARD                      (0x01) // 0
#define L_FORWARD                      (0x02) // 1
#define R_REVERSE                      (0x04) // 2
#define L_REVERSE                      (0x08) // 3
#define LCD_BACKLITE                   (0x10) // 4
#define P6_5                           (0x20) // 5
#define GRN_LED                        (0x40) // 6

// clocks.c macros
#define MCLK_FREQ_MHZ                  (8)        // MCLK = 8MHz
#define CLEAR_REGISTER                 (0X0000)

// main.c macros
#define RED_LED_ON                     (P1OUT |= RED_LED)
#define RED_LED_OFF                    (P1OUT &= ~RED_LED)
#define GREEN_LED_ON                   (P6OUT |= GRN_LED)
#define GREEN_LED_OFF                  (P6OUT &= ~GRN_LED)

// Indicies
#define ZERO                             (0)
#define ONE                              (1)
#define TWO                              (2)
#define THREE                            (3)
#define FOUR                             (4)
#define FIVE                             (5)
#define SIX                              (6)
#define TEN                              (10)

#define R_DRIVE                           (P6OUT |= R_FORWARD)
#define L_DRIVE                           (P6OUT |= L_FORWARD)
