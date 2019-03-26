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

/**
 * @brief Computed directional drive state
 *
 * Three channels are defined:
 * 1. IMBALANCE_LEFT: car should drive right
 * 2. BALANCED: car should drive straight
 * 3. IMBALANCE_RIGHT: car should drive left
 * 4. INVALID_FLS: invalid car drive state
 */
typedef enum follow_line_state_t {
  FLS_MIN,
  NO_LINE = FLS_MIN,
  LEFT_OF_LINE,
  RIGHT_OF_LINE,
  SIDEWAYS,
  FLS_MAX = SIDEWAYS,
  INVALID_FLS
} follow_line_state;

ADC_LOCAL_DEF volatile follow_line_state fl_state;

/**
 * @brief Initializes the ADC parameters
 *
 * The following ADC ports are initialized
 * V_DETECT_L Port 1 Pin 2 (0x04)
 * V_DETECT_R Port 1 Pin 3 (0x08)
 * V_THUMB    Port 1 Pin 5 (0x20)
 */
void init_adc(void);

/**
 * @brief update the fl_state global variable
 *
 */
void update_follow_line_state(void);

#endif /* ADC_HEADER_H */