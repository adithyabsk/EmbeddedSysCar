/** @file adc.h
 *  @brief Function prototypes for the Analog to Digital Convertor.
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef ADC_HEADER_H
#define ADC_HEADER_H

#ifndef ADC_LOCAL_DEF
#define ADC_LOCAL_DEF extern
#endif

#include <stdint.h>

/**
 * @brief Computed directional drive state
 *
 * Three channels are defined:
 * 1. IMBALANCE_LEFT: car should drive right
 * 2. BALANCED: car should drive straight
 * 3. IMBALANCE_RIGHT: car should drive left
 * 4. INVALID_FLS: invalid car drive state
 */
enum follow_line_state {
  NO_LINE,
  LEFT_OF_LINE,
  RIGHT_OF_LINE,
  SIDEWAYS,
  INVALID_FLS
};

__no_init ADC_LOCAL_DEF volatile enum follow_line_state fl_state;

/*
 * State variable for thumb wheel.
 */
__no_init ADC_LOCAL_DEF volatile unsigned int adc_thmb;

/*
 * State variable for left detector.
 */
__no_init ADC_LOCAL_DEF volatile unsigned int adc_ldet;

/*
 * State variable for right detector.
 */
__no_init ADC_LOCAL_DEF volatile unsigned int adc_rdet;

/**
 * @brief Initializes the ADC parameters
 *
 * The following ADC ports are initialized
 * V_DETECT_L Port 1 Pin 2 (0x04)
 * V_DETECT_R Port 1 Pin 3 (0x08)
 * V_THUMB    Port 1 Pin 5 (0x20)
 */
extern inline void init_adc(void);

/**
 * @brief update the fl_state global variable
 *
 */
void update_follow_line_state(void);

#endif /* ADC_HEADER_H */