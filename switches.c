//------------------------------------------------------------------------------
//
//  Description: This file controls the switches on the board
//
//
//  Adithya Balaji
//  Feb 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "macros.h"
#include  "functions.h"
#include  "msp430.h"

void Switches_Process(void);

extern volatile unsigned int Time_Sequence;

void Switches_Process(void)
{
    // static int sw1_unlatch = UNLATCH;
    // static int sw2_unlatch = UNLATCH;
    // static int sw1_latch_time = INIT;
    // static int sw2_latch_time = INIT;
    // 
    // // Process Switch values
    // int raw_sw1 = !(P4IN & SW1);
    // int raw_sw2 = !(P2IN & SW2);
    // int filtered_sw1 = raw_sw1 & sw1_unlatch;
    // int filtered_sw2 = raw_sw2 & sw2_unlatch;
    // 
    // // Check latch reset
    // if(sw1_latch_time > sw1_latch_time + LATCH_TIME_DELTA) {
    //     sw1_latch_time = INIT;
    //     sw1_unlatch = UNLATCH;
    // }
    // if(sw2_latch_time > sw2_latch_time + LATCH_TIME_DELTA) {
    //     sw2_latch_time = INIT;
    //     sw2_unlatch = UNLATCH;
    // }
    // 
    // // Set switch conditions
    // if(filtered_sw1) {
    //     // MCLKReset;
    //     // SMCLKReset;
    //     CSCTL5 |= DIVM__1;
    //     CSCTL5 |= DIVS__1;
    //     Init_Ports_3(USE_GPIO);
    //     sw1_latch_time = Time_Sequence;
    //     sw1_unlatch = INIT;
    // }

    // if(filtered_sw2) {
    //     // MCLK4MHz;
    //     // SMCLK500kHz;
    //     CSCTL5 |= DIVM__2;
    //     CSCTL5 |= DIVS__8;
    //     Init_Ports_3(USE_SMCLK);
    //     sw2_latch_time = Time_Sequence;
    //     sw2_unlatch = INIT;
    // }

    if (!(P4IN & SW1)) {
        CSCTL5 &= ~DIVM__2;
        CSCTL5 &= ~DIVS__8;
        Init_Ports_3(USE_GPIO);
        CSCTL5 |= DIVM__1;
        CSCTL5 |= DIVS__1;
    }
    if (!(P2IN & SW2)) {  
        Init_Ports_3(USE_SMCLK);
        CSCTL5 |= DIVM__2;
        CSCTL5 |= DIVS__8;
    }
}