/** @file adc.c
 *  @brief Analog to Digital Convertor initialization setup.
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define ADC_LOCAL_DEF
#include "adc.h"

#include "msp430.h"

#include "common.h"

#define IR_TOLERANCE (0x100)
#define BLACK (0x500)

enum adc_state adc_channel = ADC_STATE_MIN;

void init_adc(void) {
  // V_DETECT_L Port 1 Pin 2 (0x04)
  // V_DETECT_R Port 1 Pin 3 (0x08)
  // V_THUMB    Port 1 Pin 5 (0x20)

  // ADCCTL1 & ADCBUSY  identifies a conversion is in process

  // ADCCTL0 Register
  ADCCTL0 = INIT_CLEAR;  // Reset
  ADCCTL0 |= ADCSHT_2;   // 16 ADC Clocks
  ADCCTL0 |= ADCMSC;     // MSC ()
  ADCCTL0 |= ADCON;      // ADC on

  // ADCCTL1 Register
  ADCCTL1 = INIT_CLEAR;    // Reset
  ADCCTL1 |= ADCSHS_0;     // 00b = ADCSC bit
  ADCCTL1 |= ADCSHP;       // ADC sample-and-hold SAMPCON signal from
                           // sampling timer.
  ADCCTL1 &= ~ADCISSH;     // ADC invert signal sample-and-hold.
  ADCCTL1 |= ADCDIV_0;     // ADC clock divider - 000b = Divide by 1
  ADCCTL1 |= ADCSSEL_0;    // ADC clock MODCLK
  ADCCTL1 |= ADCCONSEQ_0;  // ADC conversion sequence 00b = Single-channel
                           // single-conversion

  // ADCCTl2 Register
  ADCCTL2 = INIT_CLEAR;  // Reset
  ADCCTL2 |= ADCPDIV0;   // ADC pre-divider 00b = Pre-divide by 1
  ADCCTL2 |= ADCRES_2;   // ADC resolution 10b = 12 bit
                         // (14 clock cycle conversion time)
  ADCCTL2 &= ~ADCDF;     // ADC data read-back format 0b = Binary unsigned.
  ADCCTL2 &= ~ADCSR;     // ADC sampling rate 0b = ADC buffer supports
                         // up to 200 ksps

  // ADCMCTL0 Register
  ADCMCTL0 |= ADCSREF_0;  // VREF - 000b = {VR+ = AVCC and VR– = AVSS }
  ADCMCTL0 |= ADCINCH_5;  // V_THUMB (0x20) Pin 5 A5

  ADCIE |= ADCIE0;    // Enable ADC conv complete interrupt
  ADCCTL0 |= ADCENC;  // ADC enable conversion.
  ADCCTL0 |= ADCSC;   // ADC start conversion.
}

void update_follow_line_state(void) {
  int left = adc_ldet;
  int right = adc_rdet;
  if ((right > BLACK) && (left > BLACK)) {
    fl_state = SIDEWAYS;
  } else if ((right > BLACK) && (left <= BLACK)) {
    fl_state = LEFT_OF_LINE;
  } else if ((right <= BLACK) && (left > BLACK)) {
    fl_state = RIGHT_OF_LINE;
  } else {
    fl_state = NO_LINE;
  }
}

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
          ADCMCTL0 &= ~ADCINCH_5;  // Turn off V_THUMB (0x20) Pin 5
          ADCMCTL0 |= ADCINCH_2;   // Turn on LDET (0x04) Pin 2
          break;
        case LDET_CHANNEL:
          adc_ldet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_2;  // Turn off LDET (0x04) Pin 2
          ADCMCTL0 |= ADCINCH_3;   // Turn on RDET (0x08) Pin 3
          break;
        case RDET_CHANNEL:
          adc_rdet = ADCMEM0;
          ADCMCTL0 &= ~ADCINCH_3;  // Turn off RDET (0x08) Pin 3
          ADCMCTL0 |= ADCINCH_5;   // Turn on V_THUMB (0x20) Pin 5
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