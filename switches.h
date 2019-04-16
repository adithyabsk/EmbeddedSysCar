/** @file switches.h
 *  @brief Global variable declarations for switches_interrupt
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdint.h>

#ifndef SWITCHES_LOCAL_DEF
#define SWITCHES_LOCAL_DEF extern
#endif

__no_init SWITCHES_LOCAL_DEF volatile unsigned int sw1_pressed;
__no_init SWITCHES_LOCAL_DEF volatile unsigned int sw2_pressed;

extern inline void init_switches(void);

#endif /* SWITCHES_INTERRUPT_H */