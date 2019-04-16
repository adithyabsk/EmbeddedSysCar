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

enum cmd_state { CMD_NONE, CMD_RECEIVING, CMD_INVALID };

void init_serial_iot(void);
void init_serial_usb(void);
void process_serial_stream(volatile char[], char cmd[], unsigned int*,
                           volatile unsigned int*, unsigned int*,
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

void iot_transmit(char* tx) {
  int i;
  for (i = 0; i < strlen(tx); i++) {
    iot_const_out(tx[i]);
  }
}

/*
Debugging character dropping:
* usb transmit does not recieve the dropped character
* char_rx has all of the characters
* Error is somewhere between the read in and the transmit
*/

void usb_transmit(char* tx) {
  if (usb_transmit_state == SET_TRANSMIT_ON) {
    int i;
    for (i = 0; i < strlen(tx); i++) {
      usb_const_out((char)tx[i]);
    }
  }
}

inline void init_serial(void) {
  // Init IOT
  iot_rx_ring_wr = BEGINNING;
  iot_rx_ring_rd = BEGINNING;
  memset((char*)iot_char_rx, 0, SMALL_RING_SIZE);

  iot_cmd_idx = BEGINNING;
  memset(iot_cmd, 0, CMD_BUFFER);

  fill_iot_resp_buff = BOOLEAN_FALSE;
  memset(iot_resp_buff, 0, RESP_BUFFER);
  iot_resp_buff_idx = 0;

  iot_state = CMD_NONE;

  // Configure UART0
  UCA0CTLW0 = INIT_CLEAR;      // Use word register
  UCA0CTLW0 |= UCSWRST;        // Set software reset enable
  UCA0CTLW0 |= UCSSEL__SMCLK;  // Set SMCLK as F_BRCLK

  // set_iot_baud_rate(BAUD_115200);
  UCA0BRW = B115200_BRx;
  UCA0MCTLW = B115200_MCTLW;

  UCA0CTLW0 &= ~UCSWRST;  // Set software reset enable
  UCA0IE |= UCRXIE;       // Enable Rx interrupt
  UCA0IE &= ~UCTXIE;      // Disable Tx interrupt

  // Init USB
  usb_transmit_state = SET_TRANSMIT_OFF;

  usb_rx_ring_wr = BEGINNING;
  usb_rx_ring_rd = BEGINNING;
  memset((char*)usb_char_rx, 0, SMALL_RING_SIZE);

  usb_cmd_idx = BEGINNING;
  memset(usb_cmd, 0, CMD_BUFFER);

  usb_state = CMD_NONE;

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

  enable_usb_loopback = BOOLEAN_FALSE;
}

/*
enum response_status set_iot_baud_rate(enum baud_state b) {
  switch (b) {
    case BAUD_9600:
      UCA0BRW = B9600_BRx;
      UCA0MCTLW = B9600_MCTLW;
      return PROCESS_BAUD_9600;
    case BAUD_115200:
      UCA0BRW = B115200_BRx;
      UCA0MCTLW = B115200_MCTLW;
      return PROCESS_BAUD_115200;
    case BAUD_460800:
      UCA0BRW = B460800_BRx;
      UCA0MCTLW = B460800_MCTLW;
      return PROCESS_BAUD_460800;
    default:
      return PROCESS_FAILED;
  }
}

enum response_status set_usb_baud_rate(enum baud_state b) {
  switch (b) {
    case BAUD_9600:
      UCA1BRW = B9600_BRx;
      UCA1MCTLW = B9600_MCTLW;
      return PROCESS_BAUD_9600;
    case BAUD_115200:
      UCA1BRW = B115200_BRx;
      UCA1MCTLW = B115200_MCTLW;
      return PROCESS_BAUD_115200;
    case BAUD_460800:
      UCA1BRW = B460800_BRx;
      UCA1MCTLW = B460800_MCTLW;
      return PROCESS_BAUD_460800;
    default:
      return PROCESS_FAILED;
  }
}
*/

void update_serial_states(void) {
  process_serial_stream(iot_char_rx, iot_cmd, &iot_cmd_idx, &iot_rx_ring_wr,
                        &iot_rx_ring_rd, &iot_state, '$', T_IOT);
  process_serial_stream(usb_char_rx, usb_cmd, &usb_cmd_idx, &usb_rx_ring_wr,
                        &usb_rx_ring_rd, &usb_state, '^', T_USB);
}

void serial_passthrough(char str_buffer[CMD_BUFFER], int length,
                        enum transmit_device td) {
  char transmit_str[CMD_BUFFER];
  memset(transmit_str, 0, CMD_BUFFER);
  strncpy(transmit_str, str_buffer, length);
  switch (td) {
    case T_USB:
      iot_transmit(transmit_str);  // USB Rx --> IOT Tx
      break;
    case T_IOT:
      usb_transmit(transmit_str);  // IOT Tx --> USB Rx
      break;
    default:
      break;
  }
}

unsigned int read_char(volatile char* char_rx, unsigned int* rx_rd_ptr,
                       volatile unsigned int* rx_wr_ptr, char* out) {
  unsigned int _rx_rd = *rx_rd_ptr;
  unsigned int _rx_wr = *rx_wr_ptr;
  if (_rx_rd != _rx_wr) {
    *out = char_rx[_rx_rd];
    if (++(_rx_rd) >= (SMALL_RING_SIZE - 1)) {
      _rx_rd = 0;
    }
    *rx_rd_ptr = _rx_rd;
    return BOOLEAN_TRUE;
  } else {
    return BOOLEAN_FALSE;
  }
}

void add_cmd(char* cmd) {  // end_ptr + 1 - cmd
  memset((char*)(cmd_list + cmd_list_wr), 0, CMD_BUFFER);
  strcpy(cmd_list[cmd_list_wr], cmd);
  if (++cmd_list_wr > (CMD_MAX_SIZE - 1)) {
    cmd_list_wr = 0;
  }
}

void process_serial_stream(volatile char char_rx[SMALL_RING_SIZE],
                           char cmd[CMD_MAX_SIZE], unsigned int* cmd_idx_ptr,
                           volatile unsigned int* rx_wr_ptr,
                           unsigned int* rx_rd_ptr, enum cmd_state* state,
                           char command_char, enum transmit_device td) {
  char* start_ptr;
  char* end_ptr;
  int cmd_len;

  char read_buffer[CMD_BUFFER];
  int i = 0;
  memset(read_buffer, 0, CMD_BUFFER);
  while ((i < (CMD_BUFFER - 1)) &&
         read_char(char_rx, rx_rd_ptr, rx_wr_ptr, &(read_buffer[i]))) {
    i++;
  }
  int extra_cmd_txt;
  int buff_len = strlen(read_buffer);
  if ((td == T_IOT) && (fill_iot_resp_buff)) {
    if (buff_len <= (RESP_BUFFER - 1 - iot_resp_buff_idx)) {
      char* curr_char;
      for (curr_char = read_buffer; (curr_char - read_buffer) < buff_len;
           curr_char++) {
        iot_resp_buff[iot_resp_buff_idx++] = *curr_char;
      }
    } else {
      fill_iot_resp_buff = BOOLEAN_FALSE;
    }
  }

  do {
    extra_cmd_txt = 0;
    switch (*state) {
      case CMD_NONE:
        start_ptr = strchr(read_buffer, command_char);
        if (start_ptr) {
          memset(cmd, 0, CMD_BUFFER);
          *cmd_idx_ptr = BEGINNING;
          unsigned int pass_len = (unsigned int)(start_ptr - read_buffer);
          if (pass_len) {
            serial_passthrough(read_buffer, pass_len, td);
          }
          strcpy(cmd, start_ptr);
          *cmd_idx_ptr = (unsigned int)strlen(start_ptr);
          *state = CMD_RECEIVING;
        } else if (buff_len) {
          serial_passthrough(read_buffer, CMD_BUFFER, td);
        }
        break;
      case CMD_RECEIVING:
        strncpy((char*)(cmd + *cmd_idx_ptr), read_buffer,
                CMD_BUFFER - *cmd_idx_ptr);
        *cmd_idx_ptr = *cmd_idx_ptr + strlen(read_buffer);
        end_ptr = strchr(cmd, '\r');
        cmd_len = strlen(cmd);
        if (end_ptr) {
          memset((char*)(cmd_list + cmd_list_wr), 0, CMD_BUFFER);
          strncpy(cmd_list[cmd_list_wr], cmd, end_ptr + 1 - cmd);
          if ((end_ptr - cmd) != cmd_len - 1) {
            extra_cmd_txt = 1;
            memset(read_buffer, 0, CMD_BUFFER);
            strcpy(read_buffer, end_ptr);
          }
          *cmd_idx_ptr = 0;
          if (++cmd_list_wr > (CMD_MAX_SIZE - 1)) {
            cmd_list_wr = 0;
          }
          *state = CMD_NONE;
        } else if (cmd_len > (CMD_BUFFER - 1)) {
          memset(cmd, 0, CMD_BUFFER);
          *cmd_idx_ptr = 0;
          *state = CMD_NONE;
        }
        break;
      default:
        break;
    }
  } while (extra_cmd_txt);
}

/*
char* process_status(enum response_status ps) {
  static char b9600[7] = "9_600\r";
  static char b115200[9] = "115_200\r";
  static char b460800[9] = "460_800\r";
  static char ok[4] = "OK\r";
  static char failed[8] = "FAILED\r";
  static char invalid[9] = "INVALID\r";

  switch (ps) {
    case PROCESS_OK:
      return ok;
    case PROCESS_BAUD_9600:
      return b9600;
    case PROCESS_BAUD_115200:
      return b115200;
    case PROCESS_BAUD_460800:
      return b460800;
    case PROCESS_FAILED:
      return failed;
    default:
      return invalid;
  }
}
*/

/*
void process_fram_cmd(char* cmd) {
  if (!strcmp(cmd, "^\r")) {
    usb_transmit(process_status(PROCESS_OK));
  } else if (!strcmp(cmd, "^F\r")) {
    usb_transmit(process_status(set_iot_baud_rate(BAUD_115200)));
  } else if (!strcmp(cmd, "^S\r")) {
    usb_transmit(process_status(set_iot_baud_rate(BAUD_9600)));
  }
}
*/

void process_drive_cmd(char* cmd) {
  if (strlen(cmd) == 3) {
    switch (cmd[1]) {
      case 'w':
        car_drive_state = DRIVE_FORWARD;
        break;
      case 'a':
        car_drive_state = DRIVE_LEFT;
        break;
      case 's':
        car_drive_state = DRIVE_REVERSE;
        break;
      case 'd':
        car_drive_state = DRIVE_RIGHT;
        break;
      default:
        car_drive_state = DRIVE_NONE;
        break;
    }
  }
}

void process_commands(void) {
  if (cmd_list_wr != cmd_list_rd) {
    int i;
    char* curr_cmd;
    for (i = 0; i < abs(cmd_list_wr - cmd_list_rd); i++) {
      curr_cmd = cmd_list[(cmd_list_rd + i) % CMD_MAX_SIZE];
      if (curr_cmd[0] == '^') {  // FRAM CMDs
        // process_fram_cmd(curr_cmd);
      } else if (curr_cmd[0] == '$') {  // CAR CMDs
        process_drive_cmd(curr_cmd);
      } else if (curr_cmd[0] == '@') {  // IOT capture cmd
        iot_transmit(&(curr_cmd[1]));
        fill_iot_resp_buff = BOOLEAN_TRUE;
        memset(iot_resp_buff, 0, RESP_BUFFER);
        iot_resp_buff_idx = 0;
      }
    }
  }
  cmd_list_rd = cmd_list_wr;
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
      _irx = iot_rx_ring_wr++;
      iot_char_rx[_irx] = buf_in;
      if (iot_rx_ring_wr >= (SMALL_RING_SIZE - 1)) {
        iot_rx_ring_wr = BEGINNING;
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
        _irx = usb_rx_ring_wr++;
        usb_char_rx[_irx] = buf_in;
        if (usb_rx_ring_wr >= (SMALL_RING_SIZE - 1)) {
          usb_rx_ring_wr = BEGINNING;
        }
        if (enable_usb_loopback) {
          UCA1TXBUF = buf_in;
        }
      }
      break;
    case SERIAL_ISR_TX:  // Vector 4 - TxIFG
      break;
    default:
      break;
  }
}
