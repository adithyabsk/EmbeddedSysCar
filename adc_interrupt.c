//------------------------------------------------------------------------------
//
//  Description: Configures the interrupt for processing ADC values
//
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

unsigned int adc_channel = INIT_STATE_ZERO;

volatile unsigned int adc_thmb;
volatile unsigned int adc_ldet;
volatile unsigned int adc_rdet;

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
    case ADCIV_ADCIFG:  // ADCMEM0 memory register with the conversion result
      ADCCTL0 &= ~ADCENC;     // Start next sample
      switch(++adc_channel) {
        case THUMB_CHANNEL:
          adc_thmb = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_5;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_2;  // Turn on ldet (0x20) Pin 5
          break;
        case LDET_CHANNEL:
          adc_ldet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_2;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_3;  // Turn on rdet (0x20) Pin 5
          break;
        case RDET_CHANNEL:
          adc_rdet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_3;  // V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_5;  // Turn on thumb (0x20) Pin 5
          break;
        default:
          adc_channel = INIT_STATE_ZERO;
          break;
      }
      ADCCTL0 |= ADCENC;  // ADC enable conversion.
      ADCCTL0 |= ADCSC;   // Start next sample
      break;
    default:
      break;
  }
}

// Notes make the ADC_Thumb generic
// use adc init end setup to change which value is read in
// use requirements clock cycle to select value that is read in
// set the value that is read in using the port pin number