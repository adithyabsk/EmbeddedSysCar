/** @file iot.h
 *  @brief IOT headers
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef IOT_H
#define IOT_H

#include <stdint.h>
#include "common.h"

#ifndef IOT_LOCAL_DEF
#define IOT_LOCAL_DEF extern
#endif

#define MAX_KEY_SIZE (10)
#define MAX_VALUE_SIZE (20)

enum ifconfig_values {
  IF_MIN,
  IF_MAC_ADDR = IF_MIN,
  IF_WSTATE,
  IF_SSID,
  IF_IP_ADDR,
  IF_SUBNET,
  IF_GATEWAY,
  IF_MAX = IF_GATEWAY,
  IF_INVALID
};

struct iot_field {
  char key[MAX_KEY_SIZE];
  char value[MAX_VALUE_SIZE];
  int display_offset;
  int max_offset;
  unsigned int scrollable;
};

__no_init IOT_LOCAL_DEF struct iot_field iot_ifconfig[enum_len(IF_MAX)];

void iotresp2ifconfig(void);
void init_iot(void);

void iot_alive(void);

#endif /* IOTH */