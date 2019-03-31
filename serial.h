/** @file serial.h
 *  @brief Serial port configuration
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SERIAL_H
#define SERIAL_H

#ifndef SERIAL_LOCAL_DEF
#define SERIAL_LOCAL_DEF extern
#endif

#define BEGINNING (0)
#define SMALL_RING_SIZE (20)
#define CMD_MAX_SIZE (30)

enum cmd_state {
  CMD_MIN,
  CMD_NONE = CMD_MIN,
  CMD_RECEIVING,
  CMD_RECEIVED,
  CMD_TRANSMITING,
  CMD_MAX = CMD_TRANSMITING,
  CMD_INVALID
};

enum baud_state {
  BAUD_MIN,
  BAUD_115200 = BAUD_MIN,
  BAUD_460800,
  BAUD_MAX = BAUD_460800,
  BAUD_INVALID
};

SERIAL_LOCAL_DEF volatile unsigned int usb_rx_ring_wr;
SERIAL_LOCAL_DEF volatile unsigned int usb_rx_ring_rd;
SERIAL_LOCAL_DEF volatile unsigned int usb_tx_ring_wr;
SERIAL_LOCAL_DEF volatile unsigned int usb_tx_ring_rd;
SERIAL_LOCAL_DEF volatile char usb_char_rx[SMALL_RING_SIZE];
SERIAL_LOCAL_DEF volatile char usb_char_tx[SMALL_RING_SIZE];

SERIAL_LOCAL_DEF volatile unsigned int iot_rx_ring_wr;
SERIAL_LOCAL_DEF volatile unsigned int iot_rx_ring_rd;
SERIAL_LOCAL_DEF volatile unsigned int iot_tx_ring_wr;
SERIAL_LOCAL_DEF volatile unsigned int iot_tx_ring_rd;
SERIAL_LOCAL_DEF volatile char iot_char_rx[SMALL_RING_SIZE];
SERIAL_LOCAL_DEF volatile char iot_char_tx[SMALL_RING_SIZE];

SERIAL_LOCAL_DEF enum baud_state system_baud;

SERIAL_LOCAL_DEF enum cmd_state usb_state;
SERIAL_LOCAL_DEF char usb_cmd[CMD_MAX_SIZE];
SERIAL_LOCAL_DEF unsigned int usb_cmd_idx;

SERIAL_LOCAL_DEF enum cmd_state iot_state;
SERIAL_LOCAL_DEF char iot_cmd[CMD_MAX_SIZE];
SERIAL_LOCAL_DEF unsigned int iot_cmd_idx;

/**
 * @brief Initializes the serial set for both A0 and A1
 *
 * This setups the Rx and Tx portions for pins A0 and A1 on port one
 * which corresponds to iot and usb communication systems,
 * respectively
 *
 */
void init_serial(void);

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

void schedule_transmit(void);

void set_iot_baud_rate(enum baud_state);
void set_usb_baud_rate(enum baud_state);

void serial_cmd_poll(void);

void clear_char_arr(char[], int);

void clear_iot_state(void);
void clear_usb_state(void);

void usb_transmit(void);
void iot_transmit(void);

#endif /* SERIAL_H */