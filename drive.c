/** @file drive.c
 *  @brief Implementation for motor drive functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "drive.h"

#include "msp430.h"

#define WHEEL_SPEED (20000)

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
  RIGHT_FORWARD_SPEED = WHEEL_SPEED;
  LEFT_FORWARD_SPEED = WHEEL_SPEED;
}

void drive_reverse(void) {
  // Turn off forward
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  // Turn on reverse
  RIGHT_REVERSE_SPEED = WHEEL_SPEED;
  LEFT_REVERSE_SPEED = WHEEL_SPEED;
}

void drive_cw(void) {
  stop_drive();
  // Turn on forward left
  LEFT_FORWARD_SPEED = WHEEL_SPEED;
  // Turn on reverse right
  RIGHT_REVERSE_SPEED = WHEEL_SPEED;
}

void drive_ccw(void) {
  stop_drive();
  // Turn on forward right
  RIGHT_FORWARD_SPEED = WHEEL_SPEED;
  // Turn on reverse left
  LEFT_REVERSE_SPEED = WHEEL_SPEED;
}