/** @file led.c
 *  @brief Implementation of serial
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SERIAL_LOCAL_DEF
#include "serial.h"

#include <stdlib.h>
#include <string.h>

#include "msp430.h"

#include "common.h"
#include "scheduler.h"
#include "switches.h"
#include "timers.h"

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

void init_serial_iot(void);
void init_serial_usb(void);
void clear_char_arr(char[], int);
void clear_volatile_char_arr(volatile char[], int);
void update_serial_state(const char[], char[], unsigned int,
                         volatile unsigned int*, enum cmd_state*);
void extract_buffer(const char[], volatile char[], unsigned int*,
                    const unsigned int, volatile unsigned int*);

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

void usb_transmit() {
  char transmit[SMALL_RING_SIZE] = "$ABCDEF";
  clear_char_arr((char*)usb_char_tx, SMALL_RING_SIZE);
  strcpy((char*)usb_char_tx, transmit);
  UCA1IE |= UCTXIE;
}

void iot_transmit() {
  char transmit[SMALL_RING_SIZE] = "$TEST_CMD ";
  clear_char_arr((char*)iot_char_tx, SMALL_RING_SIZE);
  strcpy((char*)iot_char_tx, transmit);
  UCA0IE |= UCTXIE;
}

void schedule_transmit(void) {
  VOID_FUNC_PTR ts_func = &iot_transmit;  // define function pointer
  schedule_func_call(ts_func, 10);        // wait 2 seconds
}

void init_serial(void) {
  system_baud = BAUD_115200;
  usb_state = CMD_NONE;

  init_serial_iot();
  init_serial_usb();
}

void clear_iot_state(void) {
  iot_rx_ring_wr = BEGINNING;
  iot_rx_ring_rd = BEGINNING;
  clear_volatile_char_arr(iot_char_rx, SMALL_RING_SIZE);  // Init IOT Rx Buffer

  iot_tx_ring_wr = BEGINNING;
  iot_tx_ring_rd = BEGINNING;
  clear_volatile_char_arr(iot_char_tx, SMALL_RING_SIZE);  // Init IOT Tx Buffer

  iot_cmd_idx = BEGINNING;
  clear_char_arr(iot_cmd, CMD_MAX_SIZE);

  iot_state = CMD_NONE;
}

void clear_usb_state(void) {
  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  clear_volatile_char_arr(usb_char_rx, SMALL_RING_SIZE);  // Init USB Rx Buffer

  usb_tx_ring_wr = BEGINNING;
  usb_tx_ring_rd = BEGINNING;
  clear_volatile_char_arr(usb_char_tx, SMALL_RING_SIZE);  // Init USB Tx Buffer

  usb_cmd_idx = BEGINNING;
  clear_char_arr(usb_cmd, CMD_MAX_SIZE);

  usb_state = CMD_NONE;
}

void init_serial_iot(void) {
  clear_iot_state();

  // Configure UART0
  UCA0CTLW0 = INIT_CLEAR;      // Use word register
  UCA0CTLW0 |= UCSWRST;        // Set software reset enable
  UCA0CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  set_iot_baud_rate(system_baud);
  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
  UCA0IE &= ~UCTXIE;      // Disable Tx interrupt
}

void init_serial_usb(void) {
  clear_usb_state();

  // Configure UART0
  UCA1CTLW0 = INIT_CLEAR;      // Use word register
  UCA1CTLW0 |= UCSWRST;        // Set software reset enable
  UCA1CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  set_usb_baud_rate(system_baud);
  UCA1CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA1IE |= UCRXIE;       // Enable Rx interrupt
  UCA1IE &= ~UCTXIE;      // Disable Tx interrupt
}

void clear_char_arr(char arr[], int length) {
  int i;
  for (i = INIT_CLEAR; i < length; i++) {
    arr[i] = INIT_CLEAR;
  }
}

void clear_volatile_char_arr(volatile char arr[], int length) {
  int i;
  for (i = INIT_CLEAR; i < length; i++) {
    arr[i] = INIT_CLEAR;
  }
}

void set_iot_baud_rate(enum baud_state b) {
  switch (b) {
    case BAUD_115200:
      UCA0BRW = B115200_BRx;
      UCA0MCTLW = B115200_MCTLW;
      break;
    case BAUD_460800:
      UCA0BRW = B460800_BRx;
      UCA0MCTLW = B460800_MCTLW;
    default:
      break;
  }
}

void set_usb_baud_rate(enum baud_state b) {
  switch (b) {
    case BAUD_115200:
      UCA1BRW = B115200_BRx;
      UCA1MCTLW = B115200_MCTLW;
      break;
    case BAUD_460800:
      UCA1BRW = B460800_BRx;
      UCA1MCTLW = B460800_MCTLW;
    default:
      break;
  }
}

void serial_cmd_poll(void) {
  update_serial_state((const char*)usb_char_rx, usb_cmd, usb_cmd_idx,
                      &usb_rx_ring_rd, &usb_state);
  update_serial_state((const char*)iot_char_rx, iot_cmd, iot_cmd_idx,
                      &iot_rx_ring_rd, &iot_state);

  switch (usb_state) {
    case CMD_RECEIVING:
      extract_buffer((const char*)usb_char_rx, usb_cmd, &usb_cmd_idx,
                     usb_rx_ring_wr, &usb_rx_ring_rd);
      break;
    case CMD_RECEIVED:
      // set transmit interrupt enable
      break;
    default:
      break;
  }
  switch (iot_state) {
    case CMD_RECEIVING:
      extract_buffer((const char*)iot_char_rx, iot_cmd, &iot_cmd_idx,
                     iot_rx_ring_wr, &iot_rx_ring_rd);
      break;
    case CMD_RECEIVED:
      // set transmit interrupt enable
      break;
    default:
      break;
  }
}

void update_serial_state(const char char_rx[SMALL_RING_SIZE],
                         char cmd[CMD_MAX_SIZE], unsigned int cmd_idx,
                         volatile unsigned int* ring_rd,
                         enum cmd_state* state) {
  char* end_ptr;
  char* start_ptr;
  switch (*state) {
    case CMD_NONE:
      start_ptr = strchr(char_rx, '$');
      int a = 10;
      if (start_ptr) {
        *ring_rd = (int)(start_ptr - char_rx);
        *state = CMD_RECEIVING;
      }
      break;
    case CMD_RECEIVING:
      end_ptr = strchr(cmd, (char)(4));
      if (end_ptr) {
        *end_ptr = '\0';  // set EOT back to null
        *state = CMD_RECEIVED;
      }
      break;
    default:
      break;
  }
}

void extract_buffer(const char buf[SMALL_RING_SIZE],
                    volatile char out[CMD_MAX_SIZE], unsigned int* cmd_idx,
                    const unsigned int wr_idx, volatile unsigned int* rd_idx) {
  unsigned int i;
  for (i = INIT_CLEAR; i < abs(wr_idx - (*rd_idx)); i++) {
    out[(*cmd_idx)++] = buf[(((*rd_idx) + i) % SMALL_RING_SIZE)];
  }
  *rd_idx = wr_idx;
}

#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
  unsigned int _irx;
  unsigned int _itx;
  char buf_in;

  switch (__even_in_range(UCA0IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      _irx = iot_rx_ring_wr++;
      buf_in = UCA0RXBUF;
      if (buf_in == '\0') {
        iot_char_rx[_irx] = (char)(4);  // End of Transmission (EOT)
      } else {
        iot_char_rx[_irx] = buf_in;
      }

      if (iot_rx_ring_wr >= (sizeof(iot_char_rx))) {
        iot_rx_ring_wr = BEGINNING;
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      _itx = iot_tx_ring_wr++;
      UCA0TXBUF = iot_char_tx[_itx];
      if ((iot_tx_ring_wr >= (sizeof(iot_char_tx))) ||
          (iot_char_tx[_itx] == '\0')) {
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
  char buf_in;

  switch (__even_in_range(UCA1IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      _irx = usb_rx_ring_wr++;
      buf_in = UCA1RXBUF;
      if (buf_in == '\0') {
        usb_char_rx[_irx] = (char)(4);  // End of Transmission (EOT)
      } else {
        usb_char_rx[_irx] = buf_in;
      }
      if (usb_rx_ring_wr >= (sizeof(usb_char_rx))) {
        usb_rx_ring_wr = BEGINNING;
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      _itx = usb_tx_ring_wr++;
      UCA1TXBUF = usb_char_tx[_itx];
      if ((usb_tx_ring_wr >= (sizeof(usb_char_tx))) ||
          (usb_char_tx[_itx] == '\0')) {
        usb_tx_ring_wr = BEGINNING;
        UCA1IE &= ~UCTXIE;
      }
      break;
    default:
      break;
  }
}
