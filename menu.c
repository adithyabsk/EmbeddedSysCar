/** @file menu.c
 *  @brief Implementation of a menu functionality
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "menu.h"

#include <stdint.h>
#include <string.h>

#include "msp430.h"

#include "adc.h"
#include "common.h"
#include "display.h"
#include "display_text.h"
#include "iot.h"
#include "ports.h"
#include "scheduler.h"
#include "serial.h"
#include "switches.h"
#include "timers.h"

#define THMB_SCROLL_OFFSET (400)
#define HEX_STR_LEN (3)
#define RPL_VAL_OFFSET (5)

#define BACK_BTN_PTR (&sw1_pressed)
#define SEL_BTN_PTR (&sw2_pressed)

unsigned int latch_thmb_value;

enum menu_main_pages {
  MAIN_MENU_MIN,
  MAIN_MENU_DIAG = MAIN_MENU_MIN,
  MAIN_MENU_RUN,
  MAIN_MENU_BLTEST,
  MAIN_MENU_NONE2,
  MAIN_MENU_MAX = MAIN_MENU_NONE2,
  MAIN_MENU_NONE
};

const char menu_main_page_strs[enum_len(MAIN_MENU_MAX)][DISP_TEXT_SIZE] = {
    "  CONFIG  ", "   RUN    ", "  BLTEST  ", EMPTY_STR};

int hover_main_item = MAIN_MENU_MIN;
int curr_main_item = MAIN_MENU_NONE;

enum menu_diag_pages {
  DIAG_MENU_MIN,
  DIAG_MENU_THMB = DIAG_MENU_MIN,
  DIAG_MENU_RDET,
  DIAG_MENU_LDET,
  DIAG_MENU_CLK,
  DIAG_MENU_IR,
  DIAG_MENU_MAX = DIAG_MENU_IR,
  DIAG_MENU_NONE
};

char menu_diag_page_strs[enum_len(DIAG_MENU_MAX)][DISP_TEXT_SIZE] = {
    " TMB(xxx) ", " RDT(xxx) ", " LDT(xxx) ", " CK xxx.x ",
    " IR (xxx) "};  // specifically modifiable (not const)

int hover_diag_item = DIAG_MENU_MIN;
int curr_diag_item = DIAG_MENU_NONE;

// Prototypes
void scroll_control(int, int, int*);
void checkset_btn_press(volatile unsigned int*, int*, int, VOID_FUNC_PTR);
void update_diag_values(void);
void diag_menu_state_controller(void);
void config_menu_state_controller(void);
void run_menu_state_controller(void);
void menu_state_controller(void);

VOID_FUNC_PTR init_scroll_ptr = &init_scroll;
inline void init_scroll(void) {
  latch_thmb_value = adc_thmb;
  *BACK_BTN_PTR = INIT_CLEAR;
  *SEL_BTN_PTR = INIT_CLEAR;
}

static void scroll_control(int min_item, int max_item, int* curr_item) {
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

static void checkset_btn_press(volatile unsigned int* btn, int* curr_item,
                               int set_value, VOID_FUNC_PTR side_effect) {
  if (*btn) {
    *btn = INIT_CLEAR;
    if (curr_item) {
      *curr_item = set_value;
    }
    if (side_effect) {
      (*(side_effect))();
    }
  }
}

static void update_diag_values(void) {
  int2hex4bit(adc_ldet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_LDET]);
  int2hex4bit(adc_rdet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_RDET]);
  int2hex4bit(adc_thmb, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_THMB]);
  state2str(IR_LED_STATE, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_IR]);
  walltime2dec(wall_clock_time_count, TIME_VAL_OFFSET,
               menu_diag_page_strs[DIAG_MENU_CLK]);
}

static void diag_menu_state_controller(void) {
  switch (curr_diag_item) {
    case DIAG_MENU_NONE:
      update_diag_values();
      scroll_control(DIAG_MENU_MIN, DIAG_MENU_MAX, &hover_diag_item);
      if (hover_diag_item == DIAG_MENU_IR) {
        display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                       hover_diag_item, BOOLEAN_TRUE);
      } else {
        display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                       hover_diag_item, BOOLEAN_FALSE);
      }
      checkset_btn_press(SEL_BTN_PTR, &curr_diag_item, hover_diag_item,
                         init_scroll_ptr);
      break;
    case DIAG_MENU_IR:
      IR_LED_TOGGLE;
      curr_diag_item = DIAG_MENU_NONE;
      break;
    default:
      curr_diag_item = DIAG_MENU_NONE;
  }
  if (curr_diag_item != DIAG_MENU_NONE) {
    checkset_btn_press(BACK_BTN_PTR, &curr_diag_item, DIAG_MENU_NONE,
                       init_scroll_ptr);
  }
}

void menu_state_controller(void) {
  switch (curr_main_item) {
    case MAIN_MENU_NONE:
      scroll_control(MAIN_MENU_MIN, MAIN_MENU_MAX, (int*)&hover_main_item);
      display_scroll(menu_main_page_strs, enum_len(MAIN_MENU_MAX),
                     hover_main_item, BOOLEAN_TRUE);
      checkset_btn_press(SEL_BTN_PTR, &curr_main_item, hover_main_item,
                         init_scroll_ptr);
      break;
    case MAIN_MENU_DIAG:
      diag_menu_state_controller();
      break;
    case MAIN_MENU_RUN:
      display_run_status();
      break;
    case MAIN_MENU_BLTEST:

      break;
    default:
      curr_main_item = MAIN_MENU_NONE;  // selection not implemented
      break;
  }
  if (curr_main_item != MAIN_MENU_NONE) {
    checkset_btn_press(BACK_BTN_PTR, &curr_main_item, MAIN_MENU_NONE,
                       init_scroll_ptr);
  }
}