/** @file iot.c
 *  @brief Implementation for iot processing functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define IOT_LOCAL_DEF
#include "iot.h"

#include <string.h>

#include "display.h"
#include "ports.h"
#include "scheduler.h"
#include "serial.h"

#define MIN(a, b) ((a < b) ? a : b)

enum iot_rst_state { SET_RESET_IOT, SET_ENABLE_IOT, INVALID_IOT_RST_STATE };

const char ifconfig_search_strs[enum_len(IF_MAX)][MAX_KEY_SIZE] = {
    "MAC=", "WSTATE=", " SSID=", "IP addr=", "SubNet=", "Gateway="};

const char WHITESPACE[] = " \f\n\r\t\v";

void init_iot_ifconfig(void) {
  int i;
  for (i = 0; i < enum_len(IF_MAX); i++) {
    memset(iot_ifconfig[i].key, 0, MAX_KEY_SIZE);
    memset(iot_ifconfig[i].value, 0, MAX_VALUE_SIZE);
    strcpy(iot_ifconfig[i].key, ifconfig_search_strs[i]);
    iot_ifconfig[i].display_offset = 0;
    iot_ifconfig[i].max_offset = 0;
  }
}

void capval(char* key, char* value) {
  char* found_key = strstr(iot_resp_buff, key);
  if (found_key) {
    int key_len = strlen(key);
    // check if key isn't at end of buffer
    if ((int)(found_key + key_len - 1) <
        (int)(iot_resp_buff + RESP_BUFFER - 1)) {
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

void iotresp2ifconfig(void) {
  int i;
  for (i = 0; i < enum_len(IF_MAX); i++) {
    capval(iot_ifconfig[i].key, iot_ifconfig[i].value);
    int len = strlen(iot_ifconfig[i].value);
    iot_ifconfig[i].scrollable = (len > DISP_TEXT_MAX - 1);
    if (iot_ifconfig[i].scrollable) {
      iot_ifconfig[i].max_offset = len - (DISP_TEXT_MAX - 1);
    }
  }
}

void set_iot_rst_state(enum iot_rst_state irs) {
  // Port 5, pin 0
  if (irs == SET_ENABLE_IOT) {
    P5OUT |= IOT_RESET;  // High (disable reset)
  } else {
    P5OUT &= ~IOT_RESET;  // Low (synchronous low reset)
  }
}

void enable_iot(void) { set_iot_rst_state(SET_ENABLE_IOT); }

void init_iot(void) {
  VOID_FUNC_PTR ei_func = &enable_iot;
  schedule_func_call(ei_func, 1);
  init_iot_ifconfig();
}

VOID_FUNC_PTR iot_check = &iot_alive;
void iot_alive(void) {
  static int count = 0;

  add_cmd("@AT+PING=8.8.8.8,1\r");
  add_cmd("@AT+NSTCP=9005,1\r");
  add_cmd("@AT+WSYNCINTRL=65535\r");

  if (++count < 10) {
    schedule_func_call(iot_check, 100);
  }
}