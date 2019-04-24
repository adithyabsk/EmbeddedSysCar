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

#define SERIAL_RING_SIZE (30)
#define RESP_BUFF_SIZE (250)

struct serial_ring_buff {
  volatile char buff[SERIAL_RING_SIZE];
  int rd_idx;  // not modified in ISR
  volatile int wr_idx;
};

struct resp_buff {
  volatile char buff[RESP_BUFF_SIZE];
  int wr_idx;  // only modified by ISR
  volatile int should_cap;
};

enum transmit_device { T_USB, T_IOT, T_INVALID };

enum transmit_state { SET_TRANSMIT_OFF, SET_TRANSMIT_ON };

SERIAL_LOCAL_DEF struct serial_ring_buff iot_ring_buff;
SERIAL_LOCAL_DEF struct serial_ring_buff usb_ring_buff;

SERIAL_LOCAL_DEF struct resp_buff iot_resp_buff;

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
 * @brief Transmits a null terminated string through the USB serial interface
 *
 */
void usb_transmit(char*);

/**
 * @brief Transmits a null terminated string through the IOT serial interface
 *
 */
void iot_transmit(char*);

/**
 * @brief Places the selected unread buff into a passed unread character array
 *
 */
int read_buff(struct serial_ring_buff*, char[SERIAL_RING_SIZE]);

/**
 * @brief Prepares IOT response buffer for data capture
 *
 */
void init_resp_buff(struct resp_buff*, int);

#endif /* SERIAL_H */