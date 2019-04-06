/** @file menu.c
 *  @brief Implementation of a menu functionality
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "menu.h"

#include <string.h>

#include "msp430.h"

#include "adc.h"
#include "common.h"
#include "display.h"
#include "display_text.h"
#include "ports.h"
#include "serial.h"
#include "switches.h"

#define THMB_SCROLL_OFFSET (600)
#define HEX_STR_LEN (3)
#define RPL_VAL_OFFSET (5)
#define menu_len(MAX_VAL) (MAX_VAL + 1)

unsigned int latch_thmb_value;

enum menu_main_pages {
  MAIN_MENU_MIN,
  MAIN_MENU_BOOT = MAIN_MENU_MIN,
  MAIN_MENU_DIAG,
  MAIN_MENU_CONFIG,
  MAIN_MENU_RUN,
  MAIN_MENU_MAX = MAIN_MENU_RUN,
  MAIN_MENU_NONE
};

const char menu_main_page_strs[menu_len(MAIN_MENU_MAX)][DISP_TEXT_MAX] = {
    " BT  DISP ", "   DIAG   ", "  CONFIG  ", "   RUNx   "};

int hover_main_item = MAIN_MENU_MIN;
int curr_main_item = MAIN_MENU_NONE;

enum menu_diag_pages {
  DIAG_MENU_MIN,
  DIAG_MENU_LDET = DIAG_MENU_MIN,
  DIAG_MENU_RDET,
  DIAG_MENU_THMB,
  DIAG_MENU_IR,
  DIAG_MENU_RX_USB_DATA,
  DIAG_MENU_RX_USB_HEADER,
  DIAG_MENU_RX_IOT_DATA,
  DIAG_MENU_RX_IOT_HEADER,
  DIAG_MENU_MAX = DIAG_MENU_RX_IOT_HEADER,
  DIAG_MENU_INVALID
};

char menu_diag_page_strs[menu_len(DIAG_MENU_MAX)][DISP_TEXT_MAX] = {
    " LDT(xxx) ", " RDT(xxx) ", " TMB(xxx) ", " IR (xxx) ", "          ",
    " RX   USB ", "          ", " RX   IOT "};  // specifically modifiable (not
                                                // const)

int hover_diag_item = DIAG_MENU_MIN;

enum menu_config_pages {
  CONFIG_MENU_MIN,
  CONFIG_MENU_IR = CONFIG_MENU_MIN,
  CONFIG_MENU_WHL_VALS,
  CONFIG_MENU_PID_VALS,
  CONFIG_MENU_TEST_SWs,
  CONFIG_MENU_TSTtxUSB,
  CONFIG_MENU_TSTtxIOT,
  CONFIG_MENU_MAX = CONFIG_MENU_TSTtxIOT,
  CONFIG_MENU_NONE
};

const char menu_config_page_strs[menu_len(CONFIG_MENU_MAX)][DISP_TEXT_MAX] = {
    " IR       ", " WHL VALS ", " PID VALS ",
    " TEST SWS ", " TSTtxUSB ", " TSTtxIOT "};

int hover_config_item = CONFIG_MENU_MIN;
int curr_config_item = CONFIG_MENU_NONE;

enum menu_run_pages {
  RUN_MENU_MIN,
  RUN_MENU_INT_LINE = RUN_MENU_MIN,
  RUN_MENU_FRWD,
  RUN_MENU_BKWD,
  RUN_MENU_TIMED_FWD,
  RUN_MENU_TIMED_BKD,
  RUN_MENU_MAX = RUN_MENU_TIMED_BKD,
  RUN_MENU_NONE
};

const char menu_run_page_strs[menu_len(RUN_MENU_MAX)][DISP_TEXT_MAX] = {
    " INT LINE ", "   FRWD   ", "   BKWD   ", "  T_FRWD  ", "  T_BKWD  ",
};

int hover_run_item = RUN_MENU_MIN;
int curr_run_item = RUN_MENU_NONE;

void init_scroll(void) { latch_thmb_value = adc_thmb; }

void scroll_control(int min_item, int max_item, int* curr_item) {
  if (adc_thmb > (latch_thmb_value + THMB_SCROLL_OFFSET)) {
    if (*curr_item >= max_item) {
      *curr_item = min_item;
    } else {
      (*curr_item)++;
    }
    latch_thmb_value = adc_thmb;
  } else if (adc_thmb < latch_thmb_value) {
    latch_thmb_value = adc_thmb;
  }
}

void check_back_menu_press(int* curr_item, int set_value) {
  if (sw1_pressed) {
    sw1_pressed = INIT_CLEAR;
    init_scroll();
    *curr_item = set_value;
  }
}

void update_diag_values(void) {
  int2hex4bit(adc_ldet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_LDET]);
  int2hex4bit(adc_rdet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_RDET]);
  int2hex4bit(adc_thmb, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_THMB]);
  state2str(IR_LED_STATE, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_IR]);
  strcpy(menu_diag_page_strs[DIAG_MENU_RX_USB_DATA], (char*)usb_char_rx);
  strcpy(menu_diag_page_strs[DIAG_MENU_RX_IOT_DATA], (char*)iot_char_rx);
}

void diag_menu_state_controller(void) {
  update_diag_values();
  scroll_control(DIAG_MENU_MIN, DIAG_MENU_MAX, &hover_diag_item);
  display_scroll(menu_diag_page_strs, menu_len(DIAG_MENU_MAX), hover_diag_item,
                 BOOLEAN_FALSE);
  check_back_menu_press((int*)&curr_main_item, MAIN_MENU_NONE);
}

void menu_state_controller(void) {
  if (curr_main_item == MAIN_MENU_NONE) {
    scroll_control(MAIN_MENU_MIN, MAIN_MENU_MAX, (int*)&hover_main_item);
    display_scroll(menu_main_page_strs, menu_len(MAIN_MENU_MAX),
                   hover_main_item, BOOLEAN_TRUE);
    if (sw2_pressed) {
      sw2_pressed = INIT_CLEAR;
      curr_main_item = hover_main_item;  // item selected
      init_scroll();
    }
  } else {  // Item was selected
    switch (curr_main_item) {
      case MAIN_MENU_BOOT:
        boot_display();
        check_back_menu_press((int*)&curr_main_item, MAIN_MENU_NONE);
        break;
      case MAIN_MENU_DIAG:
        diag_menu_state_controller();
        break;
      default:
        curr_main_item = MAIN_MENU_NONE;  // selection not implemented
        break;
    }
  }
  // Clear all switch presses that weren't captured
  sw1_pressed = INIT_CLEAR;
  sw2_pressed = INIT_CLEAR;
}