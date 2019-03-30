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

/*
 * Boolean state variable for the ir led.
 */
SWITCHES_LOCAL_DEF volatile unsigned int ir_status;

#endif /* SWITCHES_INTERRUPT_H */