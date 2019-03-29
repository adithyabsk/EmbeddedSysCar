/** @file led.c
 *  @brief Implementation of serial
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SERIAL_LOCAL_DEF

#include "serial.h"

#include <string.h>

#include "msp430.h"

void init_serial_uca0(void);
void init_serial_uca1(void);
void baud_rate_setup_a0_115200(void);
void baud_rate_setup_a0_460800(void);
void baud_rate_setup_a1_115200(void);
void baud_rate_setup_a1_460800(void);

extern volatile unsigned int wall_clock_time_count;
extern volatile unsigned int switch_press_time;

void out_character(char character) {
  // include a timeout after this works
  while (!(UCA1IFG & UCTXIFG))
    ;  // USCI_A1 TX buffer ready?
  UCA1TXBUF = character;
}

void transmit_character(void) {
  unsigned int _swt = switch_press_time;
  unsigned int _wctc = wall_clock_time_count;
  if ((_swt + 10 > _wctc) && (_wctc > 25)) {
    char transmit[11] = " NCSU  #1 ";
    int i;
    for (i = 0; i < 11; i++) {
      while (!(UCA1IFG & UCTXIFG))
        ;  // USCI_A1 TX buffer ready?
      UCA1TXBUF = transmit[i];
    }
  }
}

void init_serial(void) {
  uca0_index = BEGINNING;
  uca1_index = BEGINNING;

  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;

  iot_rx_ring_wr = BEGINNING;
  iot_rx_ring_rd = BEGINNING;
  iot_tx_ring_wr = BEGINNING;
  iot_tx_ring_rd = BEGINNING;

  strcpy((char*)test_command, "hello     ");
  init_serial_uca0();
  init_serial_uca1();
}

void init_serial_uca0(void) {
  int i;

  for (i = 0; i < SMALL_RING_SIZE; i++) {  // Init USB Rx Buffer
    iot_char_rx[i] = 0x00;
  }

  iot_rx_ring_wr = BEGINNING;
  iot_rx_ring_rd = BEGINNING;

  for (i = 0; i < SMALL_RING_SIZE; i++) {  // Iinit USB Tx Buffer
    iot_char_tx[i] = 0x00;
  }

  iot_tx_ring_wr = BEGINNING;
  iot_tx_ring_rd = BEGINNING;

  // Configure UART0
  UCA0CTLW0 = 0;               // Use word register
  UCA0CTLW0 |= UCSWRST;        // Set software reset enable
  UCA0CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  baud_rate_setup_a0_115200();

  // Init Tx
  // UCA0_index = 0;
  // UCA0IE |= UCTXIE; // Enable RX interrupt
  // UCA0TXBUF = test_command[0];
}

void init_serial_uca1(void) {
  int i;

  for (i = 0; i < SMALL_RING_SIZE; i++) {  // Init USB Rx Buffer
    usb_char_rx[i] = 0x00;
  }

  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;

  for (i = 0; i < SMALL_RING_SIZE; i++) {  // Iinit USB Tx Buffer
    usb_char_tx[i] = 0x00;
  }

  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;

  // Configure UART0
  UCA1CTLW0 = 0;               // Use word register
  UCA1CTLW0 |= UCSWRST;        // Set software reset enable
  UCA1CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  baud_rate_setup_a1_115200();

  // Init Tx
  // UCA1_index = 0;
  // UCA1IE |= UCTXIE; // Enable RX interrupt
  // UCA1TXBUF = test_command[0];
}

void baud_rate_setup_a0_115200(void) {
  UCA0BRW = 4;
  UCA0MCTLW = 0x5551;
  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
}

void baud_rate_setup_a0_460800(void) {
  UCA0BRW = 1;
  UCA0MCTLW = 0x4A11;
  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
}

void baud_rate_setup_a1_115200(void) {
  UCA1BRW = 4;
  UCA1MCTLW = 0x5551;
  UCA1CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA1IE |= UCRXIE;       // Enable Rx interrupt
}

void baud_rate_setup_a1_460800(void) {
  UCA1BRW = 1;
  UCA1MCTLW = 0x4A11;
  UCA1CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA1IE |= UCRXIE;       // Enable Rx interrupt
}
