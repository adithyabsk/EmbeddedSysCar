/** @file display_text.c
 *  @brief Implementation for display functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "display_text.h"

#include <string.h>

#include "msp430.h"

#include "commands.h"
#include "common.h"
#include "display.h"
#include "drive.h"
#include "iot.h"
#include "timers.h"

#define HEX_MAX_STR_LEN (3)
#define HEX_BASE (16)
#define TIME_STR_LEN (5)
#define TIME_LSB_IDX (4)
#define TIME_LSB_DIVISOR (5)
#define TIME_NONDEC_LSB_IDX (2)
#define TIME_DIVISOR (10)
#define TIME_LSB_MULT (2)
#define ASCII_OFFSET (48)
#define IP_ADDR_OFFSET (3)

#define offset(var, n) (var + n)

const char checkpoint_strs[enum_len(CHECK_LIST_MAX)][DISP_TEXT_SIZE] = {
    "Arrived 00", "Arrived 01", "Arrived 02", "Arrived 03", "Arrived 04",
    "Arrived 05", "Arrived 06", "Arrived 07", "Arrived 08", EMPTY_STR};

void clear_display(void) {
  display_screen(NULL_STR, NULL_STR, NULL_STR, NULL_STR, BOOLEAN_TRUE);
}

void display_scroll(const char config_list[][DISP_TEXT_SIZE], int list_len,
                    int curr_item, int display_selector) {
  char sel_elem[DISP_TEXT_SIZE];
  strcpy(sel_elem, config_list[curr_item]);

  if (display_selector) {  // Always scroll
    sel_elem[0] = '<';
    sel_elem[9] = '>';
  }
  if (list_len <= DISP_MAX_ROWS) {  // Move selector
    display_screen((curr_item == 0) ? sel_elem : config_list[0],
                   (curr_item == 1) ? sel_elem : config_list[1],
                   (curr_item == 2) ? sel_elem : config_list[2],
                   (curr_item == 3) ? sel_elem : config_list[3], BOOLEAN_TRUE);
  } else {  // Move display
    unsigned int _cio3 = offset(curr_item, 3);
    unsigned int _cio2 = offset(curr_item, 2);
    unsigned int _cio1 = offset(curr_item, 1);
    display_screen(
        config_list[(_cio3 >= list_len) ? (_cio3 % list_len) : _cio3],
        config_list[(_cio2 >= list_len) ? (_cio2 % list_len) : _cio2],
        config_list[(_cio1 >= list_len) ? (_cio1 % list_len) : _cio1], sel_elem,
        BOOLEAN_TRUE);
  }
}

char dec2hex(int n) {
  switch (n) {
    case 10:
      return 'A';
    case 11:
      return 'B';
    case 12:
      return 'C';
    case 13:
      return 'D';
    case 14:
      return 'E';
    case 15:
      return 'F';
    default:
      return n + ASCII_OFFSET;
  }
}

void int2hex4bit(int input, int offset, char* out) {
  char hex[HEX_MAX_STR_LEN];
  int i;
  int temp_input = input;
  for (i = offset(HEX_MAX_STR_LEN, -1); i >= INIT_CLEAR; i--) {
    hex[i] = dec2hex(temp_input % HEX_BASE);
    temp_input /= HEX_BASE;
  }
  strncpy((char*)(out + offset), hex, sizeof(hex));
}

void state2str(int state, int offset, char* out) {
  if (state) {
    strncpy((char*)(out + offset), "ON ", 3);
  } else {
    strncpy((char*)(out + offset), "OFF", 3);
  }
}

void walltime2dec(int time, int offset, char* out) {
  char dec_data[TIME_STR_LEN] = "xxx.x";
  dec_data[TIME_LSB_IDX] =
      ((time % TIME_LSB_DIVISOR) * TIME_LSB_MULT) + ASCII_OFFSET;
  time /= TIME_LSB_DIVISOR;

  for (int i = TIME_NONDEC_LSB_IDX; i >= INIT_CLEAR; i--) {
    dec_data[i] = (time % TIME_DIVISOR) + ASCII_OFFSET;
    time /= TIME_DIVISOR;
    ;
  }
  strncpy((char*)(out + offset), dec_data, sizeof(dec_data));
}

void cleanse_inline_null(char* str, int str_len) {
  char* c;
  for (c = str; (int)(c - str) < str_len; c++) {
    if (*c == NULL_CHAR) *c = ' ';
  }
}

void display_run_status(void) {
  char time_str[DISP_TEXT_SIZE] = " CK xxx.x ";
  walltime2dec(wall_clock_time_count - drive_start_time, TIME_VAL_OFFSET,
               time_str);
  switch (car_run_status) {
    case RS_WAITING:
      display_screen(" Waiting  ", "    for   ", "  Input   ", EMPTY_STR,
                     BOOLEAN_FALSE);
      break;
    case RS_ARCADE:
      display_screen(checkpoint_strs[curr_checkpoint], most_recent_cmd,
                     &iot_ifconfig[IF_IP_ADDR].value[IP_ADDR_OFFSET], time_str,
                     BOOLEAN_FALSE);
      break;
    case RS_AUTONOMOUS:
      break;
  }
}

void truncate_disp_str(char out[DISP_TEXT_SIZE], char* input) {
  memset(out, ' ', DISP_TEXT_SIZE);
  out[max_str_len(DISP_TEXT_SIZE)] = NULL_CHAR;
  int str_len = MIN(strlen(input), max_str_len(DISP_TEXT_SIZE));
  strncpy(out, input, str_len);
}