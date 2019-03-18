/** @file adc_interrupt.h
 *  @brief Analog to Digital Convertor ISR and variables declarations
 *
 *  Defines the state variables adc variables:
 *  1. adc_thmb
 *  2. adc_ldet
 *  3. adc_rdet
 *  Each of which store the converted representation of the state of that
 *  sensor
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef ADC_INTERRUPT_HEADER_H
#define ADC_INTERRUPT_HEADER_H

#ifndef ADC_ISR_LOCAL_DEF
#define ADC_ISR_LOCAL_DEF extern
#endif

#include "msp430.h"

/*
 * State variable for thumb wheel.
 */
ADC_ISR_LOCAL_DEF volatile unsigned int adc_thmb;

/*
 * State variable for left detector.
 */
ADC_ISR_LOCAL_DEF volatile unsigned int adc_ldet;

/*
 * State variable for right detector.
 */
ADC_ISR_LOCAL_DEF volatile unsigned int adc_rdet;

/**
 * @brief All possible states for the ADC variable
 *
 * Three channels are defined:
 * 1. THUMB_CHANNEL: represents the thumb wheel
 * 2. LDET_CHANNEL: left detector
 * 3. RDET_CHANNEL: right detector
 * 4. INVALID_ADC_STATE: invalid adc state
 */
typedef enum adc_state_t {
  ADC_STATE_MIN,
  THUMB_CHANNEL = ADC_STATE_MIN,
  LDET_CHANNEL,
  RDET_CHANNEL,
  ADC_STATE_MAX = RDET_CHANNEL,
  INVALID_ADC_STATE
} adc_state;

/**
 * @brief ADC interrupt definition
 *
 * This ISR sets the ADC to capture the value
 * at each of the ports configured to work with
 * the ADC.
 *
 */
__interrupt void ADC_ISR(void);

#endif /* ADC_INTERRUPT_HEADER_H */