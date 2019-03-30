/** @file led.c
 *  @brief Implementation of serial
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SERIAL_LOCAL_DEF
#include "serial.h"

#include <string.h>

#include "msp430.h"

#include "common.h"
#include "scheduler.h"

/*  Notes on the computation of Baud Rate setting value
 *  N = fBRCLK / baud_rate (note fBRCLK is set to SMCLK)
 *  Two setup modes depending if N > or < 16
 *  if > 16
 *    BRx = int(N/16)
 *    MCTLW from docs (simple calculation)
 *  else --> LSB of MCTLW is 0 (OS16)
 *    BRx = int(N)
 *    Complex method to compute MCTLW values (hope docs has a lookup value)
 */

#define SERIAL_ISR_RANGE (0x08)
#define SERIAL_ISR_NO_INTERRUPT (0)
#define SERIAL_ISR_RX (2)
#define SERIAL_ISR_TX (4)
// Pulled from the MSP430 documentation
#define B115200_BRx (4)
#define B115200_MCTLW (0x5551)
#define B460800_BRx (17)
#define B460800_MCTLW (0x4A00)

void init_serial_uca0(void);
void init_serial_uca1(void);
void baud_rate_setup_a0_115200(void);
void baud_rate_setup_a0_460800(void);
void baud_rate_setup_a1_115200(void);
void baud_rate_setup_a1_460800(void);

extern volatile unsigned int wall_clock_time_count;
extern volatile unsigned int switch_press_time;

void iot_const_out(char c) {
  while (!(UCA0IFG & UCTXIFG)) {
    UCA0TXBUF = c;
  }
}

void usb_const_out(char c) {
  while (!(UCA1IFG & UCTXIFG)) {
    UCA1TXBUF = c;
  }
}

void transmit_string(void) {
  char transmit[11] = " NCSU  #1 ";
  strcpy((char*)usb_char_tx, transmit);
  UCA1IE |= UCTXIE;
}

void schedule_transmit(void) {
  VOID_FUNC_PTR ts_func = &transmit_string;  // define function pointer
  schedule_func_call(ts_func, 20);           // wait 2 seconds
}

void init_serial(void) {
  baud_mode = BOOLEAN_TRUE;

  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;

  init_serial_uca0();
  init_serial_uca1();
}

void init_serial_uca0(void) {
  int i;

  iot_rx_ring_wr = BEGINNING;
  iot_rx_ring_rd = BEGINNING;
  for (i = INIT_CLEAR; i < SMALL_RING_SIZE; i++) {  // Init IOT Rx Buffer
    iot_char_rx[i] = INIT_CLEAR;
  }

  iot_tx_ring_wr = BEGINNING;
  iot_tx_ring_rd = BEGINNING;
  for (i = INIT_CLEAR; i < SMALL_RING_SIZE; i++) {  // Iinit IOT Tx Buffer
    iot_char_tx[i] = INIT_CLEAR;
  }

  // Configure UART0
  UCA0CTLW0 = INIT_CLEAR;      // Use word register
  UCA0CTLW0 |= UCSWRST;        // Set software reset enable
  UCA0CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  baud_rate_setup_a0_115200();
  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
  UCA0IE &= ~UCTXIE;      // Disable Tx interrupt
}

void init_serial_uca1(void) {
  int i;

  for (i = INIT_CLEAR; i < SMALL_RING_SIZE; i++) {  // Init USB Rx Buffer
    usb_char_rx[i] = INIT_CLEAR;
  }

  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;

  for (i = INIT_CLEAR; i < SMALL_RING_SIZE; i++) {  // Iinit USB Tx Buffer
    usb_char_tx[i] = INIT_CLEAR;
  }

  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;

  // Configure UART0
  UCA1CTLW0 = INIT_CLEAR;      // Use word register
  UCA1CTLW0 |= UCSWRST;        // Set software reset enable
  UCA1CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  baud_rate_setup_a1_115200();
  UCA1CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA1IE |= UCRXIE;       // Enable Rx interrupt
  UCA1IE &= ~UCTXIE;      // Disable Tx interrupt
}

void baud_rate_setup_a0_115200(void) {
  UCA0BRW = B115200_BRx;
  UCA0MCTLW = B115200_MCTLW;
}

void baud_rate_setup_a0_460800(void) {
  UCA0BRW = B460800_BRx;
  UCA0MCTLW = B460800_MCTLW;
}

void baud_rate_setup_a1_115200(void) {
  UCA1BRW = B115200_BRx;
  UCA1MCTLW = B115200_MCTLW;
}

void baud_rate_setup_a1_460800(void) {
  UCA1BRW = B460800_BRx;
  UCA1MCTLW = B460800_MCTLW;
}

#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
  unsigned int _irx;
  unsigned int _itx;

  switch (__even_in_range(UCA0IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      _irx = iot_rx_ring_wr++;
      iot_char_rx[_irx] = UCA0RXBUF;
      if (iot_rx_ring_wr >= (sizeof(iot_char_rx))) {
        iot_rx_ring_wr = BEGINNING;
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      _itx = iot_tx_ring_wr++;
      UCA0TXBUF = iot_char_tx[_itx];
      if (iot_tx_ring_wr >= (sizeof(iot_char_tx))) {
        iot_tx_ring_wr = BEGINNING;
        UCA0IE &= ~UCTXIE;
      }
      break;
    default:
      break;
  }
}

#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
  unsigned int _irx;
  unsigned int _itx;

  switch (__even_in_range(UCA1IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      _irx = usb_rx_ring_wr++;
      usb_char_rx[_irx] = UCA1RXBUF;
      if (usb_rx_ring_wr >= (sizeof(usb_char_rx))) {
        usb_rx_ring_wr = BEGINNING;
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      _itx = usb_tx_ring_wr++;
      UCA1TXBUF = usb_char_tx[_itx];
      if (usb_tx_ring_wr >= (sizeof(usb_char_tx))) {
        usb_tx_ring_wr = BEGINNING;
        UCA1IE &= ~UCTXIE;
      }
      break;
    default:
      break;
  }
}
