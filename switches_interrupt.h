/** @file switches_interrupt.h
 *  @brief Global variable declarations for switches_interrupt
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SWITCHES_INTERRUPT_H
#define SWITCHES_INTERRUPT_H

#ifndef SWITCHES_ISR_LOCAL_DEF
#define SWITCHES_ISR_LOCAL_DEF extern
#endif

/*
 * Boolean state variable for the ir led.
 */
SWITCHES_ISR_LOCAL_DEF volatile unsigned int ir_status;

#endif /* SWITCHES_INTERRUPT_H */