/** @file iot.c
 *  @brief Implementation for iot processing functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define IOT_LOCAL_DEF
#include "iot.h"

#include <string.h>

#include "common.h"
#include "display.h"
#include "ports.h"
#include "scheduler.h"
#include "serial.h"

#define MAX_IOT_CMD_SIZE (40)
#define ENABLE_IOT_DELAY (0)
#define IOT_SETUP_DELAY (50)
#define IOT_PING_DELAY (100)
#define IOT_IFCONFIG_DELAY (10)

enum iot_rst_state { SET_RESET_IOT, SET_ENABLE_IOT, INVALID_IOT_RST_STATE };

void iot_setup(void);

const char ifconfig_search_strs[enum_len(IF_MAX)][MAX_KEY_SIZE] = {
    "MAC=", "WSTATE=", " SSID=", "IP addr=", "SubNet=", "Gateway="};

const char WHITESPACE[] = " \f\n\r\t\v";

void capval(char* key, char* value) {
  char* found_key = strstr((char*)iot_resp_buff.buff, key);
  if (found_key) {
    int key_len = strlen(key);
    // check if key isn't at end of buff
    if ((int)(found_key + max_str_len(key_len)) <
        (int)(iot_resp_buff.buff + max_str_len(RESP_BUFF_SIZE))) {
      unsigned int found_value_len =
          strcspn((char*)(found_key + key_len), WHITESPACE);
      unsigned int rest_of_strlen = strlen((char*)(found_key + key_len));
      if (found_value_len != rest_of_strlen) {
        strncpy(value, (char*)(found_key + key_len),
                MIN(found_value_len, MAX_VALUE_SIZE));
      }
    }
  }
}

VOID_FUNC_PTR iotresp2ifconfig_ptr = &iotresp2ifconfig;
void iotresp2ifconfig(void) {
  int i;
  for (i = INIT_CLEAR; i < enum_len(IF_MAX); i++) {
    capval(iot_ifconfig[i].key, iot_ifconfig[i].value);
    int len = strlen(iot_ifconfig[i].value);
    iot_ifconfig[i].scrollable = (len > max_str_len(DISP_TEXT_SIZE));
    if (iot_ifconfig[i].scrollable) {
      iot_ifconfig[i].max_offset = len - max_str_len(DISP_TEXT_SIZE);
    }
  }
  iot_resp_buff.should_cap = BOOLEAN_FALSE;
}

void set_iot_rst_state(enum iot_rst_state irs) {
  // Port 5, pin 0
  if (irs == SET_ENABLE_IOT) {
    P5OUT |= IOT_RESET;  // High (disable reset)
  } else {
    P5OUT &= ~IOT_RESET;  // Low (synchronous low reset)
  }
}

VOID_FUNC_PTR iot_setup_ptr = &iot_setup;
void iot_setup(void) {
  iot_transmit("AT+WSYNCINTRL=65535\r");
  iot_transmit("AT+NSTCP=9005,1\r");
  iot_transmit("AT+NSTAT=?\r");
  init_resp_buff(&iot_resp_buff, BOOLEAN_TRUE);
  schedule_func_call(iotresp2ifconfig_ptr, IOT_IFCONFIG_DELAY);
}

void enable_iot(void) {
  set_iot_rst_state(SET_ENABLE_IOT);
  schedule_func_call(iot_setup_ptr, IOT_SETUP_DELAY);
}
VOID_FUNC_PTR ei_func = &enable_iot;

inline void init_iot(void) {
  schedule_func_call(ei_func, ENABLE_IOT_DELAY);

  int i;
  for (i = INIT_CLEAR; i < enum_len(IF_MAX); i++) {
    memset(iot_ifconfig[i].key, INIT_CLEAR, MAX_KEY_SIZE);
    memset(iot_ifconfig[i].value, INIT_CLEAR, MAX_VALUE_SIZE);
    strcpy(iot_ifconfig[i].key, ifconfig_search_strs[i]);
    iot_ifconfig[i].display_offset = INIT_CLEAR;
    iot_ifconfig[i].max_offset = INIT_CLEAR;
  }
}

VOID_FUNC_PTR iot_check = &iot_alive;
void iot_alive(void) {
  iot_transmit("AT+PING=8.8.8.8,1\r");
  schedule_func_call(iot_check, IOT_PING_DELAY);
}
