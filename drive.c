//------------------------------------------------------------------------------
//
//  Description: This file defines drive functions
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

void stop_drive(void) {
  // Turn off forward and reverse
  R_DRIVE_OFF;
  L_DRIVE_OFF;
  R_REVERSE_OFF;
  L_REVERSE_OFF;
}

void drive_forward(void) {
  // Turn off reverse
  R_REVERSE_OFF;
  L_REVERSE_OFF;
  // Turn on forward
  R_DRIVE_ON;
  L_DRIVE_ON;
}

void drive_reverse(void) {
  // Turn off forward
  R_DRIVE_OFF;
  L_DRIVE_OFF;
  // Turn on reverse
  R_REVERSE_ON;
  L_REVERSE_ON;
}

void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  L_DRIVE_ON;
  // Turn on reverse right
  R_REVERSE_ON;
}

void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  R_DRIVE_ON;
  // Turn on reverse left
  L_REVERSE_ON;
}