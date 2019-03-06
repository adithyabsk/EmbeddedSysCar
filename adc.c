//------------------------------------------------------------------------------
//
//  Description: This setups the ADC for use
//
//
//  Adithya Balaji
//  Feb 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

void init_adc(void) {
  // V_DETECT_L Port 1 Pin 2 (0x04)
  // V_DETECT_R Port 1 Pin 3 (0x08)
  // V_THUMB    Port 1 Pin 5 (0x20)

  // ADCCTL1 & ADCBUSY  identifies a conversion is in process

  // ADCCTL0 Register
  ADCCTL0 = INIT_STATE_ZERO;  // Reset
  ADCCTL0 |= ADCSHT_2;        // 16 ADC Clocks
  ADCCTL0 |= ADCMSC;          // MSC ()
  ADCCTL0 |= ADCON;           // ADC on

  // ADCCTL1 Register
  ADCCTL1 = INIT_STATE_ZERO;  // Reset
  ADCCTL1 |= ADCSHS_0;        // 00b = ADCSC bit
  ADCCTL1 |= ADCSHP;          // ADC sample-and-hold SAMPCON signal from
                              // sampling timer.
  ADCCTL1 &= ~ADCISSH;        // ADC invert signal sample-and-hold.
  ADCCTL1 |= ADCDIV_0;        // ADC clock divider - 000b = Divide by 1
  ADCCTL1 |= ADCSSEL_0;       // ADC clock MODCLK
  ADCCTL1 |= ADCCONSEQ_0;     // ADC conversion sequence 00b = Single-channel
                              // single-conversion

  // ADCCTl2 Register
  ADCCTL2 = INIT_STATE_ZERO;  // Reset
  ADCCTL2 |= ADCPDIV0;        // ADC pre-divider 00b = Pre-divide by 1
  ADCCTL2 |= ADCRES_2;        // ADC resolution 10b = 12 bit
                              // (14 clock cycle conversion time)
  ADCCTL2 &= ~ADCDF;          // ADC data read-back format 0b = Binary unsigned.
  ADCCTL2 &= ~ADCSR;          // ADC sampling rate 0b = ADC buffer supports
                              // up to 200 ksps

  // ADCMCTL0 Register
  ADCMCTL0 |= ADCSREF_0;  // VREF - 000b = {VR+ = AVCC and VR– = AVSS }
  ADCMCTL0 |= ADCINCH_5;  // V_THUMB (0x20) Pin 5 A5

  ADCIE |= ADCIE0;    // Enable ADC conv complete interrupt
  ADCCTL0 |= ADCENC;  // ADC enable conversion.
  ADCCTL0 |= ADCSC;   // ADC start conversion.
}