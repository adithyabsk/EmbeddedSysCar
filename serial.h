/** @file serial.h
 *  @brief Serial port configuration
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#ifndef SERIAL_LOCAL_DEF
#define SERIAL_LOCAL_DEF extern
#endif

#define BEGINNING (0)
#define SMALL_RING_SIZE (500)
#define CMD_MAX_SIZE (40)
#define CMD_BUFFER (30)
#define RESP_BUFFER (500)

enum baud_state {
  BAUD_MIN,
  BAUD_9600 = BAUD_MIN,
  BAUD_115200,
  BAUD_460800,
  BAUD_MAX = BAUD_460800,
  BAUD_INVALID
};

enum response_status {
  PROCESS_OK,
  PROCESS_FAILED,
  PROCESS_BAUD_9600,
  PROCESS_BAUD_115200,
  PROCESS_BAUD_460800,
  PROCESS_INVALID
};

enum transmit_device { T_USB, T_IOT, T_INVALID };

enum transmit_state { SET_TRANSMIT_OFF, SET_TRANSMIT_ON };

SERIAL_LOCAL_DEF volatile unsigned int usb_rx_ring_wr;
SERIAL_LOCAL_DEF unsigned int usb_rx_ring_rd;
SERIAL_LOCAL_DEF volatile char usb_char_rx[SMALL_RING_SIZE];

SERIAL_LOCAL_DEF volatile unsigned int iot_rx_ring_wr;
SERIAL_LOCAL_DEF unsigned int iot_rx_ring_rd;
SERIAL_LOCAL_DEF volatile char iot_char_rx[SMALL_RING_SIZE];

SERIAL_LOCAL_DEF enum cmd_state usb_state;
SERIAL_LOCAL_DEF char usb_cmd[CMD_BUFFER];
SERIAL_LOCAL_DEF unsigned int usb_cmd_idx;

SERIAL_LOCAL_DEF enum cmd_state iot_state;
SERIAL_LOCAL_DEF char iot_cmd[CMD_BUFFER];
SERIAL_LOCAL_DEF unsigned int iot_cmd_idx;

SERIAL_LOCAL_DEF volatile enum transmit_state usb_transmit_state;

SERIAL_LOCAL_DEF int enable_usb_loopback;

SERIAL_LOCAL_DEF char cmd_list[CMD_MAX_SIZE][CMD_BUFFER];
SERIAL_LOCAL_DEF unsigned int cmd_list_wr;
SERIAL_LOCAL_DEF unsigned int cmd_list_rd;

SERIAL_LOCAL_DEF unsigned int fill_iot_resp_buff;
SERIAL_LOCAL_DEF char iot_resp_buff[RESP_BUFFER];
SERIAL_LOCAL_DEF unsigned int iot_resp_buff_idx;

/**
 * @brief Initializes the serial set for both A0 and A1
 *
 * This setups the Rx and Tx portions for pins A0 and A1 on port one
 * which corresponds to iot and usb communication systems,
 * respectively
 *
 */
extern inline void init_serial(void);

/**
 * @brief Output a constant value to the iot tx register
 *
 * @note Make sure disable the TxISR
 *
 */
void iot_const_out(char);

/**
 * @brief Output a constant value to the usb tx register
 *
 * @note Make sure disable the TxISR
 *
 */
void usb_const_out(char c);

void schedule_test_transmit(void);

enum response_status set_iot_baud_rate(enum baud_state);
enum response_status set_usb_baud_rate(enum baud_state);

void update_serial_states(void);

void clear_char_arr(char[], int);

void clear_iot_state(void);
void clear_usb_state(void);

void usb_transmit(char*);
void iot_transmit(char*);

void set_usb_transmit_state(enum transmit_state);

void test_usb_loopback(void);
void test_iot_loopback(void);

void usb_test_transmit(void);
void iot_test_transmit(void);

void process_commands(void);

void add_cmd(char*);

#endif /* SERIAL_H */