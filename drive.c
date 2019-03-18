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
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

void drive_forward(void) {
  // Turn off reverse
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
  // Turn on forward
  RIGHT_FORWARD_SPEED = 20000;
  LEFT_FORWARD_SPEED = 20000;
}

void drive_reverse(void) {
  // Turn off forward
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  // Turn on reverse
  RIGHT_REVERSE_SPEED = 20000;
  LEFT_REVERSE_SPEED = 20000;
}

void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  LEFT_FORWARD_SPEED = 20000;
  // Turn on reverse right
  RIGHT_REVERSE_SPEED = 20000;
}

void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  RIGHT_FORWARD_SPEED = 20000;
  // Turn on reverse left
  LEFT_REVERSE_SPEED = 20000;
}