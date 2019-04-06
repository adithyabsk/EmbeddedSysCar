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
// #include "adc.h"
// #include "serial.h"
// #include "switches.h"
// #include "timers.h"

// #define HEX_LOW (0)
// #define HEX_TEN (10)
// #define HEX_ELEVEN (11)
// #define HEX_TWELVE (12)
// #define HEX_THIRTEEN (13)
// #define HEX_FOURTEEN (14)
// #define HEX_FIFTEEN (15)
// #define HEX_OFFSET (48)

// #define HEX_A ('A')
// #define HEX_B ('B')
// #define HEX_C ('C')
// #define HEX_D ('D')
// #define HEX_E ('E')
// #define HEX_F ('F')

// #define HEX_MAX_STR_LEN (5)
// #define HEX_STR_END (HEX_MAX_STR_LEN - 1)
// #define HEX_STR_LCHAR (HEX_MAX_STR_LEN - 2)
// #define HEX_BASE (16)

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
    display_screen(
        config_list[((curr_item - 3) < list_len) ? (curr_item - 3)
                                                 : (list_len - curr_item - 3)],
        config_list[((curr_item - 2) < list_len) ? (curr_item - 2)
                                                 : (list_len - curr_item - 2)],
        config_list[((curr_item - 1) < list_len) ? (curr_item - 1)
                                                 : (list_len - curr_item - 1)],
        sel_elem, BOOLEAN_TRUE);
  }
}

// char dec2hex(int c) {
//   switch (c) {
//     case HEX_TEN:
//       return HEX_A;
//     case HEX_ELEVEN:
//       return HEX_B;
//     case HEX_TWELVE:
//       return HEX_C;
//     case HEX_THIRTEEN:
//       return HEX_D;
//     case HEX_FOURTEEN:
//       return HEX_E;
//     case HEX_FIFTEEN:
//       return HEX_F;
//     default:
//       return c + HEX_OFFSET;
//   }
// }

// void int2hex4bit(int input, char* data) {
//   char hex[HEX_MAX_STR_LEN];
//   int i;
//   int temp_input = input;
//   for (i = HEX_STR_LCHAR; i >= HEX_LOW; i--) {
//     hex[i] = dec2hex(temp_input % HEX_BASE);
//     temp_input /= HEX_BASE;
//   }
//   hex[HEX_STR_END] = NULL_CHAR;
//   strcpy(data, hex);
// }

// void show_adc_status(void) {
//   set_clear_lines();
//
//   char thmb_disp[DISP_TEXT_MAX];
//   char ldet_disp[DISP_TEXT_MAX];
//   char rdet_disp[DISP_TEXT_MAX];
//
//   char thmb_str[HEX_MAX_STR_LEN];
//   char ldet_str[HEX_MAX_STR_LEN];
//   char rdet_str[HEX_MAX_STR_LEN];
//
//   int2hex4bit(adc_thmb, thmb_str);
//   int2hex4bit(adc_ldet, ldet_str);
//   int2hex4bit(adc_rdet, rdet_str);
//
//   strcpy(thmb_disp, "THMB: ");
//   strcpy(ldet_disp, "LDET: ");
//   strcpy(rdet_disp, "RDET: ");
//   strcat(thmb_disp, thmb_str);
//   strcat(ldet_disp, ldet_str);
//   strcat(rdet_disp, rdet_str);
//
//   strcpy(display_line[DISP_0], thmb_disp);
//   strcpy(display_line[DISP_1], ldet_disp);
//   strcpy(display_line[DISP_2], rdet_disp);
//
//   char ir_mov_status[DISP_TEXT_MAX];
//   if (ir_status) {
//     strcpy(ir_mov_status, "IR 1: ");
//   } else {
//     strcpy(ir_mov_status, "IR 0: ");
//   }
//   switch (fl_state) {
//     case LEFT_OF_LINE:
//       strcat(ir_mov_status, "(L) ");
//       break;
//     case RIGHT_OF_LINE:
//       strcat(ir_mov_status, "(R) ");
//       break;
//     default:
//       strcat(ir_mov_status, "    ");
//       break;
//   }
//   strcpy(display_line[DISP_3], ir_mov_status);
//
//   update_lines();
// }

// void walltime2dec(char* data) {
//   char dec_data[11] = "   .      ";
//
//   unsigned int curr_time = wall_clock_time_count;
//
//   dec_data[4] = ((curr_time % 5) * 2) + 48;
//   curr_time /= 5;
//
//   for (int i = 2; i >= 0; i--) {
//     dec_data[i] = (curr_time % 10) + 48;
//     curr_time /= 10;
//     ;
//   }
//   strcpy(data, dec_data);
// }

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
