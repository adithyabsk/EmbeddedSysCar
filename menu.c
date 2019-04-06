/** @file menu.c
 *  @brief Implementation of a menu functionality
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "menu.h"

#include "msp430.h"

#include "adc.h"
#include "common.h"
#include "display.h"
#include "display_text.h"
#include "switches.h"

#define THMB_SCROLL_OFFSET (600)
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

enum menu_main_pages hover_main_item = MAIN_MENU_MIN;
enum menu_main_pages curr_main_item = MAIN_MENU_NONE;

enum menu_diag_pages {
  DIAG_MENU_MIN,
  DIAG_MENU_LDET = DIAG_MENU_MIN,
  DIAG_MENU_RDET,
  DIAG_MENU_ADC,
  DIAG_MENU_IR,
  DIAG_MENU_RX_USB_HEADER,
  DIAG_MENU_RX_USB_DATA,
  DIAG_MENU_RX_IOT_HEADER,
  DIAG_MENU_RX_IOT_DATA,
  DIAG_MENU_MAX = DIAG_MENU_RX_IOT_DATA,
  DIAG_MENU_INVALID
};

const char menu_diag_page_strs[menu_len(DIAG_MENU_MAX)][DISP_TEXT_MAX] = {
    " LDT(xxx) ", " RDT(xxx) ", " ADC(xxx) ", " IR (xxx) ",
    " RX   USB ", "          ", " RX   IOT ", "          ",
};

enum menu_diag_pages curr_diag_item = DIAG_MENU_MIN;

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

enum menu_config_pages hover_config_item = CONFIG_MENU_MIN;
enum menu_config_pages curr_config_item = CONFIG_MENU_NONE;

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

enum menu_run_pages hover_run_item = RUN_MENU_MIN;
enum menu_run_pages curr_run_item = RUN_MENU_NONE;

void init_scroll(void) { latch_thmb_value = adc_thmb; }

void scroll_control(int min_item, int max_item, int* curr_item) {
  if (adc_thmb > (latch_thmb_value + THMB_SCROLL_OFFSET)) {
    if (*curr_item >= max_item) {
      *curr_item = min_item;
    } else {
      *curr_item += SINGLE_INCREMENT;
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

void menu_state_controller(void) {
  if (curr_main_item == MAIN_MENU_NONE) {
    scroll_control(MAIN_MENU_MIN, MAIN_MENU_MAX, (int*)&hover_main_item);
    display_scroll(menu_main_page_strs, menu_len(MAIN_MENU_MAX),
                   hover_main_item, BOOLEAN_TRUE);
    if (sw2_pressed) {
      sw2_pressed = INIT_CLEAR;
      curr_main_item = hover_main_item;  // item selected
    }
  } else {  // Item was selected
    switch (curr_main_item) {
      case MAIN_MENU_BOOT:
        boot_display();
        check_back_menu_press((int*)&curr_main_item, MAIN_MENU_NONE);
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