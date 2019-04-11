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
#define TIME_VAL_OFFSET (4)

#define BACK_BTN_PTR (&sw1_pressed)
#define SEL_BTN_PTR (&sw2_pressed)

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

const char menu_main_page_strs[enum_len(MAIN_MENU_MAX)][DISP_TEXT_MAX] = {
    " BT  DISP ", "   DIAG   ", "  CONFIG  ", "   RUNx   "};

int hover_main_item = MAIN_MENU_MIN;
int curr_main_item = MAIN_MENU_NONE;

enum iot_scrn_items {
  IOT_SCRN_MIN,
  IOT_SCRN_GET_STAT = IOT_SCRN_MIN,
  IOT_SCRN_PING,
  IOT_SCRN_PING_DATA,
  IOT_SCRN_MAC,
  IOT_DATA_START = IOT_SCRN_MAC,
  IOT_SCRN_MAC_DATA,
  IOT_SCRN_WSTATE,
  IOT_SCRN_WSTATE_DATA,
  IOT_SCRN_SSID,
  IOT_SCRN_SSID_DATA,
  IOT_SCRN_IP,
  IOT_SCRN_IP_DATA,
  IOT_SCRN_SUBNET,
  IOT_SCRN_SUBNET_DATA,
  IOT_SCRN_GATEWAY,
  IOT_SCRN_GATEWAY_DATA,
  IOT_DATA_END = IOT_SCRN_GATEWAY_DATA,
  IOT_SCRN_MAX = IOT_SCRN_GATEWAY_DATA,
  IOT_SCRN_NONE
};

char iot_scrn_strs[enum_len(IOT_SCRN_MAX)][DISP_TEXT_MAX] = {
    " Get Stat ", "   Ping   ", "          ", "    MAC   ", "          ",
    "  WSTATE  ", "          ", "   SSID   ", "          ", "    IP    ",
    "          ", "  SUBNET  ", "          ", " GATEWAY  ", "          "};

int hover_iot_item = IOT_SCRN_MIN;
int curr_iot_item = IOT_SCRN_NONE;
int iot_scrn_horiz_scroll = 0;

enum menu_diag_pages {
  DIAG_MENU_MIN,
  DIAG_MENU_THMB = DIAG_MENU_MIN,
  DIAG_MENU_RDET,
  DIAG_MENU_LDET,
  DIAG_MENU_CLK,
  DIAG_MENU_IR,
  DIAG_MENU_IOT,
  DIAG_MENU_MAX = DIAG_MENU_IOT,
  DIAG_MENU_NONE
};

char menu_diag_page_strs[enum_len(DIAG_MENU_MAX)][DISP_TEXT_MAX] = {
    " TMB(xxx) ", " RDT(xxx) ", " LDT(xxx) ", " CK xxx.x ",
    " IR (xxx) ", "   IOT    "};  // specifically modifiable (not const)

int hover_diag_item = DIAG_MENU_MIN;
int curr_diag_item = DIAG_MENU_NONE;

// enum menu_config_pages {
//   CONFIG_MENU_MIN,
//   CONFIG_MENU_IR = CONFIG_MENU_MIN,
//   CONFIG_MENU_WHL_VALS,
//   CONFIG_MENU_PID_VALS,
//   CONFIG_MENU_TEST_SWs,
//   CONFIG_MENU_TEST_USB_LOOPBACK,
//   CONFIG_MENU_TSTtxUSB,
//   CONFIG_MENU_TSTtxIOT,
//   CONFIG_MENU_MAX = CONFIG_MENU_TSTtxIOT,
//   CONFIG_MENU_NONE
// };
//
// const char menu_config_page_strs[enum_len(CONFIG_MENU_MAX)][DISP_TEXT_MAX] =
// {
//     " IR       ", " WHL VALS ", " PID VALS ", " TEST SWS ",
//     " TSTUSBLP ", " TSTtxUSB ", " TSTtxIOT "};
//
// int hover_config_item = CONFIG_MENU_MIN;
// int curr_config_item = CONFIG_MENU_NONE;

// enum menu_run_pages {
//   RUN_MENU_MIN,
//   RUN_MENU_INT_LINE = RUN_MENU_MIN,
//   RUN_MENU_FRWD,
//   RUN_MENU_BKWD,
//   RUN_MENU_TIMED_FWD,
//   RUN_MENU_TIMED_BKD,
//   RUN_MENU_MAX = RUN_MENU_TIMED_BKD,
//   RUN_MENU_NONE
// };
//
// const char menu_run_page_strs[enum_len(RUN_MENU_MAX)][DISP_TEXT_MAX] = {
//     " INT LINE ", "   FRWD   ", "   BKWD   ", "  T_FRWD  ", "  T_BKWD  ",
// };
//
// int hover_run_item = RUN_MENU_MIN;
// int curr_run_item = RUN_MENU_NONE;

// Prototypes
void init_scroll(void);
void scroll_control(int, int, int*);
void checkset_btn_press(volatile unsigned int*, int*, int, VOID_FUNC_PTR);
void update_diag_values(void);
void diag_menu_state_controller(void);
void config_menu_state_controller(void);
void run_menu_state_controller(void);
void menu_state_controller(void);

void init_scroll(void) {
  latch_thmb_value = adc_thmb;
  *BACK_BTN_PTR = INIT_CLEAR;
  *SEL_BTN_PTR = INIT_CLEAR;
}

void init_iot_scroll(void) {
  iot_scrn_horiz_scroll = 0;
  init_scroll();
}

// Func pointers
VOID_FUNC_PTR init_scroll_ptr = &init_scroll;
VOID_FUNC_PTR init_iot_scroll_ptr = &init_iot_scroll;

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

void checkset_btn_press(volatile unsigned int* btn, int* curr_item,
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

void update_diag_values(void) {
  int2hex4bit(adc_ldet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_LDET]);
  int2hex4bit(adc_rdet, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_RDET]);
  int2hex4bit(adc_thmb, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_THMB]);
  state2str(IR_LED_STATE, RPL_VAL_OFFSET, menu_diag_page_strs[DIAG_MENU_IR]);
  walltime2dec(wall_clock_time_count, TIME_VAL_OFFSET,
               menu_diag_page_strs[DIAG_MENU_CLK]);
  // TODO: Allow the ability to scroll into the buffers
  // strncpy(menu_diag_page_strs[DIAG_MENU_RX_USB_DATA], (char*)usb_char_rx,
  // DISP_TEXT_MAX); strncpy(menu_diag_page_strs[DIAG_MENU_RX_IOT_DATA],
  // (char*)iot_char_rx, DISP_TEXT_MAX);
}

// IOT Func Pointer (for scheduler)
VOID_FUNC_PTR ir2ic_func = &iotresp2ifconfig;

void iot_screen_state_controller(void) {
  int sel = (int)((hover_iot_item - IOT_DATA_START) / 2);
  switch (curr_iot_item) {
    case IOT_SCRN_NONE:
      display_iot_data(iot_scrn_strs, IOT_DATA_START, IOT_DATA_END);
      if (!iot_scrn_horiz_scroll) {
        scroll_control(IOT_SCRN_MIN, IOT_SCRN_MAX, &hover_iot_item);
      } else {
        scroll_control(0, iot_ifconfig[sel].max_offset,
                       &iot_ifconfig[sel].display_offset);
      }
      if ((((hover_iot_item % 2) == 1) || (hover_iot_item == 0)) &&
          iot_ifconfig[sel].scrollable) {
        display_scroll(iot_scrn_strs, enum_len(IOT_SCRN_MAX), hover_iot_item,
                       BOOLEAN_TRUE);
      } else {
        display_scroll(iot_scrn_strs, enum_len(IOT_SCRN_MAX), hover_iot_item,
                       BOOLEAN_FALSE);
      }
      checkset_btn_press(SEL_BTN_PTR, &curr_iot_item, hover_iot_item,
                         init_scroll_ptr);
      checkset_btn_press(
          BACK_BTN_PTR, &curr_diag_item, DIAG_MENU_NONE,
          init_iot_scroll_ptr);  // Custom back button configuration (oh how I
                                 // wish this were a class and I could override
                                 // the super's command)
      break;
    case IOT_SCRN_GET_STAT:
      add_cmd("@AT+NSTAT=?\r");
      schedule_func_call(ir2ic_func, 5);
      curr_iot_item = IOT_SCRN_NONE;
      break;
    // case IOT_SCRN_PING:
    case IOT_SCRN_MAC:
      // case IOT_SCRN_WSTATE:
      // case IOT_SCRN_SSID:
      // case IOT_SCRN_IP:
      // case IOT_SCRN_SUBNET:
      // case IOT_SCRN_GATEWAY:
      // scroll the view above if necessary
      if (iot_ifconfig[sel].scrollable) {
        iot_scrn_horiz_scroll = !iot_scrn_horiz_scroll;
      }
      curr_iot_item = IOT_SCRN_NONE;
      break;
    default:
      curr_iot_item = IOT_SCRN_NONE;
  }
}

void diag_menu_state_controller(void) {
  switch (curr_diag_item) {
    case DIAG_MENU_NONE:
      update_diag_values();
      scroll_control(DIAG_MENU_MIN, DIAG_MENU_MAX, &hover_diag_item);
      if (hover_diag_item == DIAG_MENU_IOT) {
        display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                       hover_diag_item, BOOLEAN_TRUE);
      } else {
        display_scroll(menu_diag_page_strs, enum_len(DIAG_MENU_MAX),
                       hover_diag_item, BOOLEAN_FALSE);
      }
      checkset_btn_press(SEL_BTN_PTR, &curr_diag_item, hover_diag_item,
                         init_scroll_ptr);
      break;
    case DIAG_MENU_IOT:
      iot_screen_state_controller();
      break;
    default:
      curr_diag_item = DIAG_MENU_NONE;
  }
  if (curr_diag_item != DIAG_MENU_NONE) {
    checkset_btn_press(BACK_BTN_PTR, &curr_diag_item, DIAG_MENU_NONE,
                       init_scroll_ptr);
  }
}

// void config_menu_state_controller(void) {
//   switch (curr_config_item) {
//     case CONFIG_MENU_NONE:
//       scroll_control(CONFIG_MENU_MIN, CONFIG_MENU_MAX,
//       (int*)&hover_config_item); display_scroll(menu_config_page_strs,
//       enum_len(CONFIG_MENU_MAX),
//                      hover_config_item, BOOLEAN_TRUE);
//       checkset_btn_press(SEL_BTN_PTR, &curr_config_item, hover_config_item,
//                          init_scroll_ptr);
//       break;
//     case CONFIG_MENU_TEST_USB_LOOPBACK:
//       checkset_btn_press(SEL_BTN_PTR, &enable_usb_loopback,
//                          !enable_usb_loopback, NULL);
//       loopback_test_display("USB LOOP: ", (char*)usb_char_rx,
//                             enable_usb_loopback);
//       checkset_btn_press(BACK_BTN_PTR, &curr_config_item, CONFIG_MENU_NONE,
//                          init_scroll_ptr);
//       break;
//     default:
//       curr_config_item = CONFIG_MENU_NONE;
//   }
// }

// void run_menu_state_controller(void) {
//   switch (curr_run_item) {
//     case RUN_MENU_NONE:
//       scroll_control(RUN_MENU_MIN, RUN_MENU_MAX, (int*)&hover_run_item);
//       display_scroll(menu_run_page_strs, enum_len(RUN_MENU_MAX),
//                      hover_run_item, BOOLEAN_TRUE);
//       checkset_btn_press(SEL_BTN_PTR, &curr_run_item, hover_run_item,
//                          init_scroll_ptr);
//       break;
//     default:
//       curr_run_item = CONFIG_MENU_NONE;
//   }
// }

void menu_state_controller(void) {
  switch (curr_main_item) {
    case MAIN_MENU_NONE:
      scroll_control(MAIN_MENU_MIN, MAIN_MENU_MAX, (int*)&hover_main_item);
      display_scroll(menu_main_page_strs, enum_len(MAIN_MENU_MAX),
                     hover_main_item, BOOLEAN_TRUE);
      checkset_btn_press(SEL_BTN_PTR, &curr_main_item, hover_main_item,
                         init_scroll_ptr);
      break;
    case MAIN_MENU_BOOT:
      boot_display();
      break;
    case MAIN_MENU_DIAG:
      diag_menu_state_controller();
      break;
    // case MAIN_MENU_CONFIG:
    //   config_menu_state_controller();
    //   break;
    // case MAIN_MENU_RUN:
    //   run_menu_state_controller();
    //   break;
    default:
      curr_main_item = MAIN_MENU_NONE;  // selection not implemented
      break;
  }
  if (curr_main_item != MAIN_MENU_NONE) {
    checkset_btn_press(BACK_BTN_PTR, &curr_main_item, MAIN_MENU_NONE,
                       init_scroll_ptr);
  }
}