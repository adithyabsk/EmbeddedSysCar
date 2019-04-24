/** @file serial.c
 *  @brief Implementation of serial
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define SERIAL_LOCAL_DEF
#include "serial.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "msp430.h"

#include "common.h"
#include "drive.h"
#include "ports.h"
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
#define B9600_BRx (52)
#define B9600_MCTLW (0x4911)
#define B115200_BRx (4)
#define B115200_MCTLW (0x5551)
#define B460800_BRx (17)
#define B460800_MCTLW (0x4A00)

#define ENABLE_TRANMIST_CHAR ('~')  // Uncommon ASCII Char

enum transmit_conn { LOOPBACK, CIRCUIT };

enum cmd_state { CMD_NONE, CMD_RECEIVING, CMD_INVALID };

void init_serial_iot(void);
void init_serial_usb(void);
void process_serial_stream(volatile char[], char cmd[], unsigned int*,
                           volatile unsigned int*, unsigned int*,
                           enum cmd_state*, char, enum transmit_device);
static void init_ring_buff(struct serial_ring_buff*);
static void iot_out_char(char);
static void usb_out_char(char);

// Global vars
static enum transmit_conn transmit_conn_state;
static volatile enum transmit_state usb_transmit_state;

// Local methods
static void iot_out_char(char c) {
  while (!(UCA0IFG & UCTXIFG))
    ;
  UCA0TXBUF = c;
}

static void usb_out_char(char c) {
  if (usb_transmit_state == SET_TRANSMIT_ON) {
    while (!(UCA1IFG & UCTXIFG))
      ;
    UCA1TXBUF = c;
  }
}

static void init_ring_buff(struct serial_ring_buff* buff) {
  buff->rd_idx = INIT_CLEAR;
  buff->wr_idx = INIT_CLEAR;
  memset((char*)(buff->buff), NULL_CHAR, SERIAL_RING_SIZE);
}

void init_resp_buff(struct resp_buff* buff, int should_cap) {
  memset((char*)(buff->buff), NULL_CHAR, RESP_BUFF_SIZE);
  buff->wr_idx = INIT_CLEAR;
  buff->should_cap = should_cap;
}

inline void init_serial(void) {
  // Loopback
  transmit_conn_state = CIRCUIT;

  // Init IOT
  init_ring_buff(&iot_ring_buff);
  init_resp_buff(&iot_resp_buff, BOOLEAN_FALSE);

  // Configure UART0
  UCA0CTLW0 = INIT_CLEAR;      // Use word register
  UCA0CTLW0 |= UCSWRST;        // Set software reset enable
  UCA0CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  UCA0BRW = B115200_BRx;
  UCA0MCTLW = B115200_MCTLW;

  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
  UCA0IE &= ~UCTXIE;      // Disable Tx interrupt

  // Init USB
  usb_transmit_state = SET_TRANSMIT_OFF;
  init_ring_buff(&usb_ring_buff);

  // Configure UART1
  UCA1CTLW0 = INIT_CLEAR;      // Use word register
  UCA1CTLW0 |= UCSWRST;        // Set software reset enable
  UCA1CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  // set_usb_baud_rate(BAUD_115200);
  UCA1BRW = B115200_BRx;
  UCA1MCTLW = B115200_MCTLW;

  UCA1CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA1IE |= UCRXIE;       // Enable Rx interrupt
  UCA1IE &= ~UCTXIE;      // Disable Tx interrupt
}

void iot_transmit(char* tx) {
  int i;
  for (i = INIT_CLEAR; i < strlen(tx); i++) {
    iot_out_char(tx[i]);
  }
}

void usb_transmit(char* tx) {
  if (usb_transmit_state == SET_TRANSMIT_ON) {
    int i;
    for (i = INIT_CLEAR; i < strlen(tx); i++) {
      usb_out_char((char)tx[i]);
    }
  }
}

int read_buff(struct serial_ring_buff* buff, char unread[SERIAL_RING_SIZE]) {
  int _wr_idx = buff->wr_idx;
  if (buff->rd_idx != _wr_idx) {
    memset(unread, NULL_CHAR, SERIAL_RING_SIZE);
    int i;
    for (i = INIT_CLEAR; i < abs(_wr_idx - buff->rd_idx); i++) {
      unread[i] = (char)buff->buff[(buff->rd_idx + i) % SERIAL_RING_SIZE];
    }
    buff->rd_idx = _wr_idx;
    return BOOLEAN_TRUE;
  } else {
    return BOOLEAN_FALSE;
  }
}

// IOT Serial Interrupt
#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
  unsigned int _irx;
  char buf_in;

  switch (__even_in_range(UCA0IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      buf_in = UCA0RXBUF;
      _irx = iot_ring_buff.wr_idx++;
      iot_ring_buff.buff[_irx] = buf_in;
      if (iot_ring_buff.wr_idx >= max_str_len(SERIAL_RING_SIZE)) {
        iot_ring_buff.wr_idx = INIT_CLEAR;
      }
      if (iot_resp_buff.should_cap) {
        iot_resp_buff.buff[iot_resp_buff.wr_idx++] = buf_in;
        if (iot_resp_buff.wr_idx > max_str_len(RESP_BUFF_SIZE)) {
          iot_resp_buff.should_cap = BOOLEAN_FALSE;
        }
      }
      switch (transmit_conn_state) {
        case CIRCUIT:
          usb_out_char(buf_in);
          break;
        default:
        case LOOPBACK:
          iot_out_char(buf_in);
          break;
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      break;
    default:
      break;
  }
}

// USB Serial Interrupt
#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
  unsigned int _irx;
  char buf_in;

  switch (__even_in_range(UCA1IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      buf_in = UCA1RXBUF;
      if ((usb_transmit_state != SET_TRANSMIT_ON) &&
          (buf_in == ENABLE_TRANMIST_CHAR)) {
        usb_transmit_state = SET_TRANSMIT_ON;
      } else {
        _irx = usb_ring_buff.wr_idx++;
        usb_ring_buff.buff[_irx] = buf_in;
        if (usb_ring_buff.wr_idx >= max_str_len(SERIAL_RING_SIZE)) {
          usb_ring_buff.wr_idx = INIT_CLEAR;
        }
        switch (transmit_conn_state) {
          case CIRCUIT:
            iot_out_char(buf_in);
            break;
          default:
          case LOOPBACK:
            usb_out_char(buf_in);
            break;
        }
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      break;
    default:
      break;
  }
}
