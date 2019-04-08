/** @file display_text.c
 *  @brief Implementation for display functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "display_text.h"

#include <string.h>

#include "msp430.h"

#include "display.h"

#include "common.h"

#define HEX_MAX_STR_LEN (3)
#define HEX_BASE (16)
#define TIME_STR_LEN (5)
#define TIME_LSB_IDX (4)
#define TIME_LSB_DIVISOR (5)
#define TIME_NONDEC_LSB_IDX (2)
#define TIME_DIVISOR (10)
#define TIME_LSB_MULT (2)
#define ASCII_OFFSET (48)

#define offset(var, n) (var + n)

void clear_display(void) {
  display_screen(NULL_STR, NULL_STR, NULL_STR, NULL_STR, BOOLEAN_TRUE);
}

void boot_display(void) {
  display_screen("EMBEDDED  ", "SYSTEMS   ", "CAR       ", EMPTY_STR,
                 BOOLEAN_FALSE);
}

void display_scroll(const char config_list[][DISP_TEXT_MAX], int list_len,
                    int curr_item, int display_selector) {
  char sel_elem[DISP_TEXT_MAX];
  strcpy(sel_elem, config_list[curr_item]);

  if (display_selector) {  // Always scroll
    sel_elem[0] = '<';
    sel_elem[9] = '>';
  } else {
    // Do nothing
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
  for (i = offset(HEX_MAX_STR_LEN, -1); i >= 0; i--) {
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

void loopback_test_display(char* header, char* rx_input, int lp_state) {
  char _rx[DISP_TEXT_MAX];
  char lp_state_str[DISP_TEXT_MAX] = "LOOP:     ";
  strncpy(_rx, rx_input, DISP_TEXT_MAX);
  state2str(lp_state, 6, lp_state_str);
  display_screen(header, "RX:       ", _rx, lp_state_str, BOOLEAN_TRUE);
}

// void show_line_follow_status(void) {
//   set_clear_lines();
//
//   char status_disp[DISP_TEXT_MAX];
//   char time_disp[DISP_TEXT_MAX];
//
//   switch (lf_routine_state) {
//     case INTERCEPTING:
//       strcpy(status_disp, "Intercept ");
//       break;
//     case WAITING:
//       strcpy(status_disp, "Waiting   ");
//       break;
//     case TURNING:
//       strcpy(status_disp, "Turning   ");
//       break;
//     case FOLLOWING_LINE:
//       strcpy(status_disp, "Circling  ");
//       break;
//     case 4:
//       strcpy(status_disp, "Stopped   ");
//       break;
//   }
//   strcpy(display_line[DISP_0], status_disp);
//   walltime2dec(time_disp);
//   strcpy(display_line[DISP_1], time_disp);
//
//   char ir_mov_status[DISP_TEXT_MAX];
//   if (ir_status) {
//     strcpy(ir_mov_status, "IR 1      ");
//   } else {
//     strcpy(ir_mov_status, "IR 0      ");
//   }
//   strcpy(display_line[DISP_3], ir_mov_status);
//
//   update_lines();
// }

// void display_baud(void) {
//   switch (iot_state) {
//     case CMD_RECEIVED:
//       display_screen(" Recieved ", NULL_STR, NULL_STR, iot_cmd,
//       BOOLEAN_TRUE); break;
//     case CMD_TRANSMITING:
//       display_screen(" Transmit ", iot_cmd, NULL_STR, NULL_STR,
//       BOOLEAN_TRUE); break;
//     default:
//       display_screen(" Waiting  ", EMPTY_STR, NULL_STR, EMPTY_STR,
//                      BOOLEAN_TRUE);
//       break;
//   }
//   switch (system_baud) {
//     case BAUD_115200:
//       set_line("  115200  ", DISP_2, BOOLEAN_FALSE);
//       break;
//     case BAUD_460800:
//       set_line("  460800  ", DISP_2, BOOLEAN_FALSE);
//       break;
//     default:
//       set_line("  ERROR   ", DISP_2, BOOLEAN_FALSE);
//       break;
//   }
//
//   update_lines();
// }
