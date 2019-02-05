//------------------------------------------------------------------------------
//
//  Description: This file initializes the ports for the board pins
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "macros.h"
#include  "functions.h"
#include  "msp430.h"

void Init_Ports(void);   // Inits the rest of the functions
void Init_Ports_1(void); // Inits Port 1
void Init_Ports_2(void); // Inits Port 2
void Init_Ports_3(void); // Inits Port 3
void Init_Ports_4(void); // Inits Port 4
void Init_Ports_5(void); // Inits Port 5
void Init_Ports_6(void); // Inits Port 6


void Init_Ports(void) {
    Init_Ports_1();
    Init_Ports_2();
    Init_Ports_3();
    Init_Ports_4();
    Init_Ports_5();
    Init_Ports_6();
}

void Init_Ports_1(void) {
    // Clear P1
    P1SEL1 = INIT;
    P1SEL0 = INIT;
    P1DIR =  OUTPUT; // Default OUT
    P1OUT =  LOW;
    P1REN =  INIT;

    // PIN 0 GPIO (00)
    P1SEL1 &= ~RED_LED;
    P1SEL0 &= ~RED_LED;
    P1OUT |= RED_LED;   // High On
    P1DIR |= RED_LED;   // Output

    // PIN 1 ADC (11)
    P1SEL1 |= A1_SEEED;
    P1SEL0 |= A1_SEEED;

    // PIN 2 UCB0SDA (01)
    P1SEL1 &= ~V_DETECT_L;
    P1SEL0 |= V_DETECT_L;

    // PIN 3 UCB0SCL (01)
    P1SEL1 &= ~V_DETECT_R;
    P1SEL0 |= V_DETECT_R;

    // PIN 4 ADC (11)
    P1SEL1 |= A4_SEEED;
    P1SEL0 |= A4_SEEED;

    // PIN 4 ADC (11)
    P1SEL1 |= A4_SEEED;
    P1SEL0 |= A4_SEEED;

    // PIN 5 ADC (11)
    P1SEL1 |= V_THUMB;
    P1SEL0 |= V_THUMB;

    // PIN 6 UCA0RXD (01)
    P1SEL1 &= ~UCA0RXD;
    P1SEL0 |= UCA0RXD;

    // PIN 7 UCA0TXD (01)
    P1SEL1 &= ~UCA0TXD;
    P1SEL0 |= UCA0TXD;
}

void Init_Ports_2(void) {
    // Clear P2
    P2SEL1 = INIT;
    P2SEL0 = INIT;
    P2DIR =  OUTPUT;
    P2OUT =  LOW;
    P2REN =  INIT;

    // PIN 0 GPIO (00)
    P2SEL1 &= ~P2_0;
    P2SEL0 &= ~P2_0;

    // PIN 1 GPIO (00)
    P2SEL1 &= ~P2_1;
    P2SEL0 &= ~P2_1;

    // PIN 2 GPIO (00)
    P2SEL1 &= ~P2_2;
    P2SEL0 &= ~P2_2;

    // PIN 3 GPIO (00)
    P2SEL1 &= ~SW2;
    P2SEL0 &= ~SW2;
    P2DIR &= ~SW2;
    P2OUT |= SW2;
    P2REN |= SW2;

    // PIN 4 GPIO (00)
    P2SEL1 &= ~P2_4;
    P2SEL0 &= ~P2_4;

    // PIN 5 GPIO (00)
    P2SEL1 &= ~P2_5;
    P2SEL0 &= ~P2_5;

    // PIN 6 XOUT (10)
    P2SEL1 |= LFXOUT;
    P2SEL0 &= ~LFXOUT;

    // PIN 7 XIN (10)
    P2SEL1 |= LFXIN;
    P2SEL0 &= ~LFXIN;
}

void Init_Ports_3(void) {
    // Clear P3
    P3SEL1 = INIT;
    P3SEL0 = INIT;
    P3DIR =  OUTPUT;
    P3OUT =  LOW;
    P3REN =  INIT;

    // PIN 0 GPIO (00)
    P3SEL1 &= ~TEST_PROBE;
    P3SEL0 &= ~TEST_PROBE;

    // PIN 1 OA2O (11)
    P3SEL1 |= OA20;
    P3SEL0 |= OA20;

    // PIN 2 OA2- (11)
    P3SEL1 |= OA2N;
    P3SEL0 |= OA2N;

    // PIN 3 OA2+ (11)
    P3SEL1 |= OA2P;
    P3SEL0 |= OA2P;

    // PIN 4 SMCLK (01)
    P3SEL1 &= ~SMCLK_OUT;
    P3SEL0 |= SMCLK_OUT;

    // PIN 5 OA3O (11)
    P3SEL1 |= OA3O;
    P3SEL0 |= OA3O;

    // PIN 6 GPIO (00)
    P3SEL1 &= ~P3_6;
    P3SEL0 &= ~P3_6;

    // PIN 7 GPIO (00)
    P3SEL1 &= ~P3_7;
    P3SEL0 &= ~P3_7;
}

void Init_Ports_4(void) {
    // Clear P4
    P4SEL1 = INIT;
    P4SEL0 = INIT;
    P4DIR =  OUTPUT;
    P4OUT =  LOW;
    P4REN =  INIT;

    // PIN 0 GPIO (00)
    P4SEL1 &= ~RESET_LCD;
    P4SEL0 &= ~RESET_LCD;
    P4OUT |= RESET_LCD;   // Low On
    P4DIR |= RESET_LCD;   // Output

    // PIN 1 GPIO (00)
    P4SEL1 &= ~SW1;
    P4SEL0 &= ~SW1;
    P4DIR &= ~SW1;
    P4OUT |= SW1;
    P4REN |= SW1;

    // PIN 2 UCA1RXD (01)
    P4SEL1 &= ~UCA1RXD;
    P4SEL0 |= UCA1RXD;

    // PIN 3 UCA1RXD (01)
    P4SEL1 &= ~UCA1TXD;
    P4SEL0 |= UCA1TXD;

    // PIN 4 UCB1STE (01)
    P4SEL1 &= ~UCB1_CS_LCD;
    P4SEL0 &= ~UCB1_CS_LCD;
    P4OUT |= UCB1_CS_LCD;
    P4DIR |= UCB1_CS_LCD;

    // PIN 5 UCB1CLK (01)
    P4SEL1 &= ~UCB1CLK;
    P4SEL0 |= UCB1CLK;

    // PIN 6 UCB1SIMO (01)
    P4SEL1 &= ~UCB1SIMO;
    P4SEL0 |= UCB1SIMO;

    // PIN 7 UCB1SOMI (01)
    P4SEL1 &= ~UCB1SOMI;
    P4SEL0 |= UCB1SOMI;
}

void Init_Ports_5(void) {
    // Clear P5
    P5SEL1 = INIT;
    P5SEL0 = INIT;
    P5DIR =  OUTPUT;
    P5OUT =  LOW;
    P5REN =  INIT;

    // PIN 0 ADC (11)
    P5SEL1 |= IOT_RESET;
    P5SEL0 |= IOT_RESET;

    // PIN 1 ADC (11)
    P5SEL1 |= IOT_LINK;
    P5SEL0 |= IOT_LINK;

    // PIN 2 ADC (11)
    P5SEL1 |= IOT_PROG_SEL;
    P5SEL0 |= IOT_PROG_SEL;

    // PIN 3 ADC (11)
    P5SEL1 |= IOT_PROG_MODE;
    P5SEL0 |= IOT_PROG_MODE;

    // PIN 4 GPIO (00)
    P5SEL1 &= ~IR_LED; // IR is a feedback loop (input)
    P5SEL0 &= ~IR_LED;
    P1DIR &= ~IR_LED;   // Input (check on this)
}

void Init_Ports_6(void) {
    // Clear P6
    P6SEL1 = INIT;
    P6SEL0 = INIT;
    P6DIR =  OUTPUT;
    P6OUT =  LOW;
    P6REN =  INIT;

    // PIN 0 TB3.1 (01)
    P6SEL1 &= ~R_FORWARD;
    P6SEL0 &= ~R_FORWARD;
    P6DIR |=  R_FORWARD; // Output

    // PIN 1 TB3.2 (01)
    P6SEL1 &= ~L_FORWARD;
    P6SEL0 &= ~L_FORWARD;
    P6DIR |=  L_FORWARD; // Output

    // PIN 2 TB3.3 (01)
    P6SEL1 &= ~R_REVERSE;
    P6SEL0 &= ~R_REVERSE;
    P6DIR |=  R_REVERSE; // Output

    // PIN 3 TB3.4 (01)
    P6SEL1 &= ~L_REVERSE;
    P6SEL0 &= ~L_REVERSE;
    P6DIR |=  L_REVERSE; // Output

    // PIN 4 TB3.5 (01)
    P6SEL1 &= ~LCD_BACKLITE;
    P6SEL0 &= ~LCD_BACKLITE;
    P6OUT |= LCD_BACKLITE;
    P6DIR |=  LCD_BACKLITE; // Output

    // PIN 5 GPIO (00)
    P6SEL1 &= ~P6_5;
    P6SEL0 &= ~P6_5;
    P6DIR |=  P6_5; // Output

    // PIN 6 GPIO (00)
    P6SEL1 &= ~GRN_LED;
    P6SEL0 &= ~GRN_LED;
    P6DIR |=  GRN_LED; // Output

    P6SEL1 &= ~GRN_LED;
    P6SEL0 &= ~GRN_LED;
    P6DIR |= GRN_LED;   // High On
    P6DIR |= GRN_LED;   // Output
}