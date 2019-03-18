/** @file adc.h
 *  @brief Function prototypes for the Analog to Digital Convertor.
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef ADC_HEADER_H
#define ADC_HEADER_H

/**
 * @brief Initializes the ADC parameters
 *
 * The following ADC ports are initialized
 * V_DETECT_L Port 1 Pin 2 (0x04)
 * V_DETECT_R Port 1 Pin 3 (0x08)
 * V_THUMB    Port 1 Pin 5 (0x20)
 */
void init_adc(void);

#endif /* ADC_HEADER_H */