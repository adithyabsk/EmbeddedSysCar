/** @file serial_interrupt.c
 *  @brief Implementation of serial interrupt
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "serial_interrupt.h"

#include "msp430.h"

#include "serial.h"

#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
  unsigned int temp;

  switch (__even_in_range(UCA0IV, 0x08)) {
    case 0:  // Vector 0 - no interrupt
      break;
    case 2:  // Vector 2 - RxIFG
      temp = iot_rx_ring_wr++;
      iot_char_rx[temp] = UCA0RXBUF;
      if (iot_rx_ring_wr >= (sizeof(iot_char_rx))) {
        iot_rx_ring_wr = BEGINNING;
      }
      break;
    case 4:  // Vector 4 - TxIFG
      switch (uca0_index++) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          UCA0TXBUF = test_command[uca0_index];
          break;
        case 9:
          UCA0TXBUF = 0x0D;
          break;
        case 10:
          UCA0TXBUF = 0x0A;
          break;
        default:
          UCA0IE &= ~UCTXIE;
          break;
      }
      break;
    default:
      break;
  }
}

#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
  unsigned int temp;

  switch (__even_in_range(UCA1IV, 0x08)) {
    case 0:  // Vector 0 - no interrupt
      break;
    case 2:  // Vector 2 - RxIFG
      temp = usb_rx_ring_wr++;
      usb_char_rx[temp] = UCA1RXBUF;
      if (usb_rx_ring_wr >= (sizeof(usb_char_rx))) {
        usb_rx_ring_wr = BEGINNING;
      }
      break;
    case 4:  // Vector 4 - TxIFG
      switch (uca1_index++) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
          UCA1TXBUF = test_command[uca1_index];
          break;
        case 9:
          UCA1TXBUF = 0x0D;
          break;
        case 10:
          UCA1TXBUF = 0x0A;
          break;
        default:
          UCA1IE &= ~UCTXIE;
          break;
      }
      break;
    default:
      break;
  }
}