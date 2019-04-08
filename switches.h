/** @file switches.h
 *  @brief Global variable declarations for switches_interrupt
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SWITCHES_H
#define SWITCHES_H

#ifndef SWITCHES_LOCAL_DEF
#define SWITCHES_LOCAL_DEF extern
#endif

SWITCHES_LOCAL_DEF volatile unsigned int sw1_pressed;
SWITCHES_LOCAL_DEF volatile unsigned int sw2_pressed;

#endif /* SWITCHES_INTERRUPT_H */