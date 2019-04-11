/** @file ports.c
 *  @brief Implementation for each of the port initializations
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "ports.h"

#include "msp430.h"

#include "common.h"
#include "scheduler.h"

static void init_port_1();
static void init_port_2();
static void init_port_3();
static void init_port_4();
static void init_port_5();
static void init_port_6();

void init_ports(void) {
  init_port_1();
  init_port_2();
  init_port_3();
  init_port_4();
  init_port_5();
  init_port_6();
}

static void init_port_1(void) {
  // Clear P1
  P1SEL1 = INIT_CLEAR;
  P1SEL0 = INIT_CLEAR;
  P1DIR = INIT_OUTPUT;  // Default OUT
  P1OUT = INIT_LOW;
  P1REN = INIT_CLEAR;

  // PIN 0 GPIO (00)
  P1SEL1 &= ~RED_LED;
  P1SEL0 &= ~RED_LED;
  P1DIR |= RED_LED;  // Output
  P1OUT |= RED_LED;  // High

  // PIN 1 ADC (11)
  P1SEL1 |= A1_SEEED;
  P1SEL0 |= A1_SEEED;

  // PIN 2 ADC (11)
  P1SEL1 |= V_DETECT_L;
  P1SEL0 |= V_DETECT_L;

  // PIN 3 ADC (11)
  P1SEL1 |= V_DETECT_R;
  P1SEL0 |= V_DETECT_R;

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

static void init_port_2(void) {
  // Clear P2
  P2SEL1 = INIT_CLEAR;
  P2SEL0 = INIT_CLEAR;
  P2DIR = INIT_OUTPUT;
  P2OUT = INIT_LOW;
  P2REN = INIT_CLEAR;

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

  // Configure SW1 interrupt
  P2IFG &= ~SW2;
  P2IE |= SW2;
  P2IES |= SW2;

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

static void init_port_3() {
  // Clear P3
  P3SEL1 = INIT_CLEAR;
  P3SEL0 = INIT_CLEAR;
  P3DIR = INIT_OUTPUT;
  P3OUT = INIT_LOW;
  P3REN = INIT_CLEAR;

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

static void init_port_4() {
  // Clear P4
  P4SEL1 = INIT_CLEAR;
  P4SEL0 = INIT_CLEAR;
  P4DIR = INIT_OUTPUT;
  P4OUT = INIT_LOW;
  P4REN = INIT_CLEAR;

  // PIN 0 GPIO (00)
  P4SEL1 &= ~RESET_LCD;
  P4SEL0 &= ~RESET_LCD;
  P4OUT |= RESET_LCD;  // Low On
  P4DIR |= RESET_LCD;  // Output

  // PIN 1 GPIO (00)
  P4SEL1 &= ~SW1;
  P4SEL0 &= ~SW1;
  P4DIR &= ~SW1;
  P4OUT |= SW1;
  P4REN |= SW1;

  // Configure SW1 interrupt
  P4IES |= SW1;
  P4IFG &= ~SW1;
  P4IE |= SW1;

  // PIN 2 UCA1RXD (01)
  P4SEL1 &= ~UCA1RXD;
  P4SEL0 |= UCA1RXD;

  // PIN 3 UCA1RXD (01)
  P4SEL1 &= ~UCA1TXD;
  P4SEL0 |= UCA1TXD;

  // PIN 4 UCB1_CS_LCD (00)
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

static void init_port_5(void) {
  // Clear P5
  P5SEL1 = INIT_CLEAR;
  P5SEL0 = INIT_CLEAR;
  P5DIR = INIT_OUTPUT;
  P5OUT = INIT_LOW;
  P5REN = INIT_CLEAR;

  // PIN 0 GPIO (00)
  P5SEL1 &= ~IOT_RESET;
  P5SEL0 &= ~IOT_RESET;
  P5DIR |= IOT_RESET;   // Output
  P5OUT &= ~IOT_RESET;  // Low

  // PIN 1 GPIO (00)
  P5SEL1 &= ~IOT_LINK;
  P5SEL0 &= ~IOT_LINK;
  P5DIR |= IOT_LINK;   // Output
  P5OUT &= ~IOT_LINK;  // Low

  // PIN 2 GPIO (00)
  P5SEL1 &= ~IOT_PROG_SEL;
  P5SEL0 &= ~IOT_PROG_SEL;
  P5DIR &= ~IOT_PROG_SEL;  // Input

  // PIN 3 GPIO (00)
  P5SEL1 &= ~IOT_PROG_MODE;
  P5SEL0 &= ~IOT_PROG_MODE;
  P5DIR &= ~IOT_PROG_MODE;  // Input

  // PIN 4 GPIO (00)
  P5SEL1 &= ~IR_LED;
  P5SEL0 &= ~IR_LED;
  P1DIR |= IR_LED;
}

static void init_port_6(void) {
  // Clear P6
  P6SEL1 = INIT_CLEAR;
  P6SEL0 = INIT_CLEAR;
  P6DIR = INIT_OUTPUT;
  P6OUT = INIT_LOW;
  P6REN = INIT_CLEAR;

  // PIN 0 TB3.1 (01)
  P6SEL1 &= ~R_FORWARD;
  P6SEL0 |= R_FORWARD;
  P6DIR |= R_FORWARD;  // Output

  // PIN 1 TB3.2 (01)
  P6SEL1 &= ~L_FORWARD;
  P6SEL0 |= L_FORWARD;
  P6DIR |= L_FORWARD;  // Output

  // PIN 2 TB3.3 (01)
  P6SEL1 &= ~R_REVERSE;
  P6SEL0 |= R_REVERSE;
  P6DIR |= R_REVERSE;  // Output

  // PIN 3 TB3.4 (01)
  P6SEL1 &= ~L_REVERSE;
  P6SEL0 |= L_REVERSE;
  P6DIR |= L_REVERSE;  // Output

  // PIN 4 TB3.5 (01)
  P6SEL1 &= ~LCD_BACKLITE;
  P6SEL0 &= ~LCD_BACKLITE;
  P6OUT |= LCD_BACKLITE;
  P6DIR |= LCD_BACKLITE;  // Output

  // PIN 5 GPIO (00)
  P6SEL1 &= ~P6_5;
  P6SEL0 &= ~P6_5;

  // PIN 6 GPIO (00)
  P6SEL1 &= ~GRN_LED;
  P6SEL0 &= ~GRN_LED;
  P6DIR |= GRN_LED;  // Output
}

void set_smclk_mode(int smclk_mode) {
  // Port 3
  // PIN 4 SMCLK (01)
  if (smclk_mode == SET_GPIO) {
    P3SEL1 &= ~SMCLK_OUT;
    P3SEL0 &= ~SMCLK_OUT;
  } else {
    P3SEL1 &= ~SMCLK_OUT;
    P3SEL0 |= SMCLK_OUT;
  }
}

void set_iot_rst_state(enum iot_rst_state irs) {
  // Port 5, pin 0
  if (irs == SET_RELEASE_IOT) {
    P5OUT |= IOT_RESET;  // High (disable reset)
  } else {
    P5OUT &= ~IOT_RESET;  // Low (synchronous low reset)
  }
}

void enable_iot(void) { set_iot_rst_state(SET_RELEASE_IOT); }

void init_iot(void) {
  VOID_FUNC_PTR ei_func = &enable_iot;
  schedule_func_call(ei_func, 1);
}