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
#include "drive.h"
#include "iot.h"
#include "ports.h"
#include "scheduler.h"
#include "serial.h"
#include "switches.h"
#include "timers.h"

#define THMB_SCROLL_OFFSET (400)
#define HEX_STR_LEN (3)
#define RPL_VAL_OFFSET (5)

#define DIVIDE_ONE (1)
#define DIVIDE_THIRTYTWO (32)

#define BACK_BTN_PTR (&sw1_pressed)
#define SEL_BTN_PTR (&sw2_pressed)

unsigned int latch_thmb_value;
unsigned int scroll_dir;  // false for vertical, true for horizontal

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
    "  CONFIG  ", "   RUN    ", EMPTY_STR};

int hover_main_item = MAIN_MENU_MIN;
int curr_main_item = MAIN_MENU_NONE;

enum menu_diag_pages {
  DIAG_MENU_MIN,
  DIAG_MENU_BL_TEST = DIAG_MENU_MIN,
  DIAG_MENU_F_ALIGN,
  DIAG_MENU_R_ALIGN,
  DIAG_MENU_P_GAIN,
  DIAG_MENU_D_GAIN,
  DIAG_MENU_THMB,
  DIAG_MENU_RDET,
  DIAG_MENU_LDET,
  DIAG_MENU_CLK,
  DIAG_MENU_IR,
  DIAG_MENU_MAX = DIAG_MENU_IR,
  DIAG_MENU_NONE
};

char menu_diag_page_strs[enum_len(DIAG_MENU_MAX)][DISP_TEXT_SIZE] = {
    "  BLTEST  ", " F +(xxx) ", " R +(xxx) ", " KP (xxx) ",
    " KD (xxx) ", " TMB(xxx) ", " RDT(xxx) ", " LDT(xxx) ",
    " CK xxx.x ", " IR (xxx) "};  // specifically modifiable (not const)

int hover_diag_item = DIAG_MENU_MIN;
int curr_diag_item = DIAG_MENU_NONE;
struct config_value* curr_diag_config = NULL;
unsigned int start_follow_line = INIT_CLEAR;

// Prototypes
void scroll_control(int, int, int*, int);
void checkset_btn_press(volatile unsigned int*, int*, int, VOID_FUNC_PTR);
void update_diag_values(void);
void diag_menu_state_controller(void);
void config_menu_state_controller(void);
void run_menu_state_controller(void);
void menu_state_controller(void);
static void init_horiz_scroll(void);

VOID_FUNC_PTR init_scroll_ptr = &init_scroll;
inline void init_scroll(void) {
  latch_thmb_value = adc_thmb;
  *BACK_BTN_PTR = INIT_CLEAR;
  *SEL_BTN_PTR = INIT_CLEAR;
}

VOID_FUNC_PTR init_horiz_scroll_ptr = &init_horiz_scroll;
static void init_horiz_scroll(void) {
  scroll_dir = INIT_CLEAR;
  init_scroll();
}

static void scroll_control(int min_item, int max_item, int* curr_item,
                           int divider) {
  if (adc_thmb > (latch_thmb_value + (int)(THMB_SCROLL_OFFSET / divider))) {
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

  int2hex4bit(p_gain.curr_val, RPL_VAL_OFFSET,
              menu_diag_page_strs[DIAG_MENU_P_GAIN]);
  int2hex4bit(d_gain.curr_val, RPL_VAL_OFFSET,
              menu_diag_page_strs[DIAG_MENU_D_GAIN]);

  state2str(IR_LED_STATE, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_IR]);
  walltime2dec(wall_clock_time_count, TIME_VAL_OFFSET,
               menu_diag_page_strs[DIAG_MENU_CLK]);
}

static void diag_menu_state_controller(void) {
  switch (curr_diag_item) {
    case DIAG_MENU_NONE:
      update_diag_values();

      if (!scroll_dir) {
        scroll_control(DIAG_MENU_MIN, DIAG_MENU_MAX, &hover_diag_item,
                       DIVIDE_ONE);
      } else {
        if (curr_diag_config) {
          scroll_control(curr_diag_config->min_val, curr_diag_config->max_val,
                         &(curr_diag_config->curr_val), DIVIDE_THIRTYTWO);
        }
      }

      switch (hover_diag_item) {
        case DIAG_MENU_IR:
        case DIAG_MENU_P_GAIN:
        case DIAG_MENU_D_GAIN:
        case DIAG_MENU_BL_TEST:
          display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                         hover_diag_item, BOOLEAN_TRUE);
          break;
        default:
          display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                         hover_diag_item, BOOLEAN_FALSE);
      }
      checkset_btn_press(SEL_BTN_PTR, &curr_diag_item, hover_diag_item,
                         init_scroll_ptr);
      break;
    case DIAG_MENU_P_GAIN:
      curr_diag_config = &p_gain;
      scroll_dir = !scroll_dir;
      curr_diag_item = DIAG_MENU_NONE;
      break;
    case DIAG_MENU_D_GAIN:
      curr_diag_config = &d_gain;
      scroll_dir = !scroll_dir;
      curr_diag_item = DIAG_MENU_NONE;
      break;
    case DIAG_MENU_BL_TEST:
      start_follow_line = !start_follow_line;
      curr_diag_item = DIAG_MENU_NONE;
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
                       init_horiz_scroll_ptr);
  }
}

void menu_state_controller(void) {
  switch (curr_main_item) {
    case MAIN_MENU_NONE:
      scroll_control(MAIN_MENU_MIN, MAIN_MENU_MAX, (int*)&hover_main_item,
                     DIVIDE_ONE);
      display_scroll(menu_main_page_strs, enum_len(MAIN_MENU_MAX),
                     hover_main_item, BOOLEAN_TRUE);
      checkset_btn_press(SEL_BTN_PTR, &curr_main_item, hover_main_item,
                         init_scroll_ptr);
      break;
    case MAIN_MENU_DIAG:
      diag_menu_state_controller();
      break;
    case MAIN_MENU_RUN:
      main_drive_controller();
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
                       init_horiz_scroll_ptr);
  }
}