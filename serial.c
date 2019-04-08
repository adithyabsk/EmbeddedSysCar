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
#define B115200_BRx (4)
#define B115200_MCTLW (0x5551)
#define B460800_BRx (17)
#define B460800_MCTLW (0x4A00)

#define ENABLE_TRANMIST_CHAR ('~')  // Uncommon ASCII Char

enum cmd_state {
  CMD_NONE,
  CMD_RECEIVING,
  CMD_RECEIVED,
  CMD_TRANSMITING,
  CMD_INVALID
};

void init_serial_iot(void);
void init_serial_usb(void);
void clear_char_arr(char[], int);
void clear_volatile_char_arr(volatile char[], int);
void process_serial_stream(volatile char[], char cmd[], unsigned int*,
                           const unsigned int, volatile unsigned int*,
                           enum cmd_state*, char, enum transmit_device);

void iot_const_out(char c) {
  while (!(UCA0IFG & UCTXIFG))
    ;
  UCA0TXBUF = c;
}

void usb_const_out(char c) {
  while (!(UCA1IFG & UCTXIFG))
    ;

  UCA1TXBUF = c;
}

void iot2waiting(void) { iot_state = CMD_NONE; }

void usb2waiting(void) { usb_state = CMD_NONE; }

void fill_ring_buffer(volatile char* dst, char* src,
                      volatile unsigned int* start_idx) {
  int str_len = strlen(src);
  if ((*start_idx + strlen(src)) <= SMALL_RING_SIZE) {
    strncpy((char*)(dst + *start_idx), src, str_len);
    *start_idx = *start_idx + str_len;
  } else {
    int offset = SMALL_RING_SIZE - *start_idx;
    strncpy((char*)(dst + *start_idx), src, offset);
    strncpy((char*)dst, (char*)(src + offset), str_len - offset);
    *start_idx = str_len - offset;
  }
}

void iot_transmit(char* tx) {
  int i;
  for (i = 0; i < strlen(tx); i++) {
    iot_const_out(tx[i]);
  }
}

void usb_transmit(char* tx) {
  if (usb_transmit_state == SET_TRANSMIT_ON) {
    int i;
    for (i = 0; i < strlen(tx); i++) {
      usb_const_out(tx[i]);
    }
  }
}

void usb_test_transmit() {
  char transmit[SMALL_RING_SIZE] = "$TEST_CMD ";
  clear_volatile_char_arr(usb_char_tx, SMALL_RING_SIZE);
  strcpy((char*)usb_char_tx, transmit);
  UCA1IE |= UCTXIE;
}

void iot_test_transmit() {
  char transmit[SMALL_RING_SIZE] = "$TEST_CMD ";
  clear_volatile_char_arr(iot_char_tx, SMALL_RING_SIZE);
  strcpy((char*)iot_char_tx, transmit);
  UCA0IE |= UCTXIE;
}

void schedule_test_transmit(void) {
  VOID_FUNC_PTR ts_func = &iot_test_transmit;  // define function pointer
  schedule_func_call(ts_func, 10);             // wait 2 seconds
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
}

void init_serial_iot(void) {
  clear_iot_state();

  iot_state = CMD_NONE;

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
  usb_transmit_state = SET_TRANSMIT_OFF;
  clear_usb_state();

  usb_state = CMD_NONE;

  // Configure UART1
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

void update_serial_states(void) {
  process_serial_stream(usb_char_rx, usb_cmd, &usb_cmd_idx, usb_rx_ring_wr,
                        &usb_rx_ring_rd, &usb_state, '^', T_USB);
  process_serial_stream(iot_char_rx, iot_cmd, &iot_cmd_idx, iot_rx_ring_wr,
                        &iot_rx_ring_rd, &iot_state, '$', T_IOT);
}

void serial_passthrough(
    volatile char char_rx[SMALL_RING_SIZE], volatile unsigned int* rx_rd_ptr,
    const unsigned int leading_index,  // could be wr_idx or start_ptr distance
    enum transmit_device td) {
  char transmit_str[SMALL_RING_SIZE];
  clear_char_arr(transmit_str, SMALL_RING_SIZE);
  if (*rx_rd_ptr < leading_index) {  // normal case (lagging read head)
    int str_len = leading_index - *rx_rd_ptr;
    char* new_ptr = (char*)(char_rx + (*rx_rd_ptr));
    strncpy(transmit_str, new_ptr, str_len);  // copy string
    memset(new_ptr, 0, str_len);              // clear string
  } else if (leading_index <
             *rx_rd_ptr) {  // infrequent case (leading read head)
    int l1 = SMALL_RING_SIZE - *rx_rd_ptr;
    char* s1 = (char*)(char_rx + (*rx_rd_ptr));
    char* t2 = (char*)(transmit_str + l1);
    strncpy(transmit_str, s1, l1);
    strncpy(t2, (char*)char_rx, leading_index);
    memset(s1, 0, l1);  // clear source one (char_rx, substring one)
    memset((char*)char_rx, 0,
           leading_index);  // clear source two (char_rx, substring two)
  } else {
    return;  // do nothing otherwise
  }
  *rx_rd_ptr = leading_index;
  // transmit the string
  switch (td) {
    case T_USB:
      usb_transmit(transmit_str);
      break;
    case T_IOT:
      iot_transmit(transmit_str);
      break;
    default:
      break;
  }
}

void process_serial_stream(volatile char char_rx[SMALL_RING_SIZE],
                           char cmd[CMD_MAX_SIZE], unsigned int* cmd_idx_ptr,
                           const unsigned int rx_wr,
                           volatile unsigned int* rx_rd_ptr,
                           enum cmd_state* state, char command_char,
                           enum transmit_device td) {
  // char* start_ptr;
  // char* end_ptr;
  switch (*state) {
    case CMD_NONE:
      // start_ptr = strchr((const char*)char_rx, command_char);
      // if (start_ptr) {
      //   clear_char_arr(cmd, CMD_MAX_SIZE);
      //   *cmd_idx_ptr = BEGINNING;
      //   unsigned int ld_idx = (int)(start_ptr - char_rx);
      //   if(ld_idx != *rx_rd_ptr) {
      //     serial_passthrough(char_rx, rx_rd_ptr, ld_idx);
      //   } else {
      //     // Do not transmit because command char is first new char
      //   }
      //   *state = CMD_RECEIVING;
      // } else {
      //   // pass through
      //   serial_passthrough(char_rx, rx_rd_ptr, rx_wr);
      // }
      serial_passthrough(char_rx, rx_rd_ptr, rx_wr, td);
      break;
    case CMD_RECEIVING:
      // end_ptr = strchr((const char*)cmd, '\r');
      // if (!end_ptr) {
      //   // read recieved data into cmd
      //   unsigned int i;
      //   for (i = INIT_CLEAR; i < abs(rx_wr - (*rx_rd_ptr)); i++) {
      //     cmd[(*cmd_idx_ptr)++] =
      //         char_rx[(((*rx_rd_ptr) + i) % SMALL_RING_SIZE)];
      //   }
      //   // set rx pointer even with write point
      //   *rx_rd_ptr = rx_wr;
      // } else if() { // reached max command size
      //   // clear command temp vars
      //   // go back to cmd_none
      // } else {
      //   // graceful ending
      //   // add command to the ledger (global vars)
      //   // clear stuff that needs cleaning
      //   // transistion back to CMD_NONE
      // }
      break;
    default:
      break;
  }
}

void process_commands(void) {
  // global vars:
  // comand list
  // command rd index,
  // command write index
}

// IOT Serial Interrupt
#pragma vector = EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
  unsigned int _irx;
  unsigned int _itx;
  char buf_in;

  switch (__even_in_range(UCA0IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      buf_in = UCA0RXBUF;
      _irx = iot_rx_ring_wr++;
      iot_char_rx[_irx] = buf_in;
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

// USB Serial Interrupt
#pragma vector = EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
  unsigned int _irx;
  unsigned int _itx;
  char buf_in;
  char transmit_char;

  switch (__even_in_range(UCA1IV, SERIAL_ISR_RANGE)) {
    case SERIAL_ISR_NO_INTERRUPT:  // Vector 0 - no interrupt
      break;
    case SERIAL_ISR_RX:  // Vector 2 - RxIFG
      buf_in = UCA1RXBUF;
      if ((usb_transmit_state != SET_TRANSMIT_ON) &&
          (buf_in == ENABLE_TRANMIST_CHAR)) {
        usb_transmit_state = SET_TRANSMIT_ON;
      } else {
        _irx = usb_rx_ring_wr++;
        usb_char_rx[_irx] = buf_in;
        if (usb_rx_ring_wr >= (sizeof(usb_char_rx))) {
          usb_rx_ring_wr = BEGINNING;
        }
        if (enable_usb_loopback) {
          UCA1TXBUF = buf_in;
        }
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
                         // _itx = usb_tx_ring_rd;
                         // transmit_char = usb_char_tx[_itx];
                         // if (
                         //   (usb_transmit_state == SET_TRANSMIT_ON)
                         //   && (_itx != usb_tx_ring_wr)
                         //   && (transmit_char != '\0')) {
                         //   UCA1TXBUF = transmit_char;
                         //   if(++usb_tx_ring_rd > SMALL_RING_SIZE-1) {
                         //     usb_tx_ring_rd = BEGINNING;
                         //   }
                         // } else {
                         //   UCA1IE &= ~UCTXIE;
                         // }
                         // break;
    default:
      break;
  }
}
