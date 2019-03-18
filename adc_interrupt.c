/** @file adc_interrupt.c
 *  @brief Analog to Digital Convertor ISR implementation
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define ADC_ISR_LOCAL_DEF
#include "adc_interrupt.h"

#include "msp430.h"

adc_state adc_channel = ADC_STATE_MIN;

#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void) {
  switch (__even_in_range(ADCIV, ADCIV_ADCIFG)) {
    case ADCIV_NONE:  // When a conversion result is written to the ADCMEM0
                      // before its previous conversion result was read.
      break;
    case ADCIV_ADCOVIFG:  // ADC conversion-time overflow
      break;
    case ADCIV_ADCTOVIFG:  // Window comparator interrupt flags
      break;
    case ADCIV_ADCHIIFG:  // Window comparator interrupt flag
      break;
    case ADCIV_ADCLOIFG:  // Window comparator interrupt flag
      break;
    case ADCIV_ADCINIFG:
      break;
    case ADCIV_ADCIFG:     // ADCMEM0 memory register with the conversion result
      ADCCTL0 &= ~ADCENC;  // Start next sample
      switch (adc_channel++) {
        case THUMB_CHANNEL:
          adc_thmb = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_5;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_2;   // Turn on ldet (0x20) Pin 5
          break;
        case LDET_CHANNEL:
          adc_ldet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_2;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_3;   // Turn on rdet (0x20) Pin 5
          break;
        case RDET_CHANNEL:
          adc_rdet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_3;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_5;   // Turn on thumb (0x20) Pin 5
          break;
        default:
          adc_channel = ADC_STATE_MIN;
          break;
      }

      ADCCTL0 |= ADCENC;  // ADC enable conversion.
      ADCCTL0 |= ADCSC;   // Start next sample
      break;
    default:
      break;
  }
}