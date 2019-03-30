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
#define SMALL_RING_SIZE (11)

SERIAL_LOCAL_DEF volatile unsigned int baud_mode;

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

void baud_rate_setup_a0_115200(void);
void baud_rate_setup_a1_115200(void);
void baud_rate_setup_a0_460800(void);
void baud_rate_setup_a1_460800(void);

#endif /* SERIAL_H */