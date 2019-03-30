/** @file display_text.c
 *  @brief Implementation for display functions
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "display_text.h"

#include <string.h>

#include "msp430.h"

#include "adc.h"
#include "common.h"
#include "serial.h"
#include "switches.h"
#include "timers.h"

#define DISP_MAX_ROWS (4)
#define DISP_TEXT_MAX (11)
#define DISP_0 (0)
#define DISP_1 (1)
#define DISP_2 (2)
#define DISP_3 (3)
#define DISPL_START (0)
#define NULL_CHAR ('\0')

#define EMPTY_STR ("          ")
#define NULL_STR ("")

#define HEX_LOW (0)
#define HEX_TEN (10)
#define HEX_ELEVEN (11)
#define HEX_TWELVE (12)
#define HEX_THIRTEEN (13)
#define HEX_FOURTEEN (14)
#define HEX_FIFTEEN (15)
#define HEX_OFFSET (48)

#define HEX_A ('A')
#define HEX_B ('B')
#define HEX_C ('C')
#define HEX_D ('D')
#define HEX_E ('E')
#define HEX_F ('F')

#define HEX_MAX_STR_LEN (5)
#define HEX_STR_END (HEX_MAX_STR_LEN - 1)
#define HEX_STR_LCHAR (HEX_MAX_STR_LEN - 2)
#define HEX_BASE (16)

// Library LCD variable access
extern char display_line[DISP_MAX_ROWS][DISP_TEXT_MAX];
extern char* display[DISP_MAX_ROWS];
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;

// Internal Prototypes
void set_clear_lines(void);
void update_lines(void);
void show_switch(char sw_status);
char dec2hex(int c);
void int2hex4bit(int input, char* data);
char move_status(void);
void display_screen(char*, char*, char*, char*, int);
void walltime2dec(char* data);
void set_line(char*, int, int);

void set_clear_lines(void) {
  strcpy(display_line[DISP_0], "          ");
  strcpy(display_line[DISP_1], "          ");
  strcpy(display_line[DISP_2], "          ");
  strcpy(display_line[DISP_3], "          ");
}

void update_lines(void) {
  update_string(display_line[DISP_0], DISP_0);
  update_string(display_line[DISP_1], DISP_1);
  update_string(display_line[DISP_2], DISP_2);
  update_string(display_line[DISP_3], DISP_3);
  display_changed = BOOLEAN_TRUE;
}

void clear_display(void) {
  set_clear_lines();
  update_lines();
}

void init_display(void) {
  int i;

  for (i = 0; i < 11; i++) {
    display_line[DISP_0][i] = INIT_CLEAR;
    display_line[DISP_1][i] = INIT_CLEAR;
    display_line[DISP_2][i] = INIT_CLEAR;
    display_line[DISP_3][i] = INIT_CLEAR;
  }
  display[DISP_0] = &display_line[DISP_0][DISPL_START];
  display[DISP_1] = &display_line[DISP_1][DISPL_START];
  display[DISP_2] = &display_line[DISP_2][DISPL_START];
  display[DISP_3] = &display_line[DISP_3][DISPL_START];

  update_display = INIT_CLEAR;
  update_display_count = INIT_CLEAR;
}

void reset_display(void) {
  strcpy(display_line[DISP_0], "   NCSU   ");
  strcpy(display_line[DISP_1], " WOLFPACK ");
  strcpy(display_line[DISP_2], "  ECE306  ");
  strcpy(display_line[DISP_3], "          ");
  update_lines();
}

char dec2hex(int c) {
  switch (c) {
    case HEX_TEN:
      return HEX_A;
    case HEX_ELEVEN:
      return HEX_B;
    case HEX_TWELVE:
      return HEX_C;
    case HEX_THIRTEEN:
      return HEX_D;
    case HEX_FOURTEEN:
      return HEX_E;
    case HEX_FIFTEEN:
      return HEX_F;
    default:
      return c + HEX_OFFSET;
  }
}

void int2hex4bit(int input, char* data) {
  char hex[HEX_MAX_STR_LEN];
  int i;
  int temp_input = input;
  for (i = HEX_STR_LCHAR; i >= HEX_LOW; i--) {
    hex[i] = dec2hex(temp_input % HEX_BASE);
    temp_input /= HEX_BASE;
  }
  hex[HEX_STR_END] = NULL_CHAR;
  strcpy(data, hex);
}

void show_adc_status(void) {
  set_clear_lines();

  char thmb_disp[DISP_TEXT_MAX];
  char ldet_disp[DISP_TEXT_MAX];
  char rdet_disp[DISP_TEXT_MAX];

  char thmb_str[HEX_MAX_STR_LEN];
  char ldet_str[HEX_MAX_STR_LEN];
  char rdet_str[HEX_MAX_STR_LEN];

  int2hex4bit(adc_thmb, thmb_str);
  int2hex4bit(adc_ldet, ldet_str);
  int2hex4bit(adc_rdet, rdet_str);

  strcpy(thmb_disp, "THMB: ");
  strcpy(ldet_disp, "LDET: ");
  strcpy(rdet_disp, "RDET: ");
  strcat(thmb_disp, thmb_str);
  strcat(ldet_disp, ldet_str);
  strcat(rdet_disp, rdet_str);

  strcpy(display_line[DISP_0], thmb_disp);
  strcpy(display_line[DISP_1], ldet_disp);
  strcpy(display_line[DISP_2], rdet_disp);

  char ir_mov_status[DISP_TEXT_MAX];
  if (ir_status) {
    strcpy(ir_mov_status, "IR 1: ");
  } else {
    strcpy(ir_mov_status, "IR 0: ");
  }
  switch (fl_state) {
    case LEFT_OF_LINE:
      strcat(ir_mov_status, "(L) ");
      break;
    case RIGHT_OF_LINE:
      strcat(ir_mov_status, "(R) ");
      break;
    default:
      strcat(ir_mov_status, "    ");
      break;
  }
  strcpy(display_line[DISP_3], ir_mov_status);

  update_lines();
}

void walltime2dec(char* data) {
  char dec_data[11] = "   .      ";

  unsigned int curr_time = wall_clock_time_count;

  dec_data[4] = ((curr_time % 5) * 2) + 48;
  curr_time /= 5;

  for (int i = 2; i >= 0; i--) {
    dec_data[i] = (curr_time % 10) + 48;
    curr_time /= 10;
    ;
  }
  strcpy(data, dec_data);
}

void show_line_follow_status(void) {
  set_clear_lines();

  char status_disp[DISP_TEXT_MAX];
  char time_disp[DISP_TEXT_MAX];

  switch (lf_routine_state) {
    case INTERCEPTING:
      strcpy(status_disp, "Intercept ");
      break;
    case WAITING:
      strcpy(status_disp, "Waiting   ");
      break;
    case TURNING:
      strcpy(status_disp, "Turning   ");
      break;
    case FOLLOWING_LINE:
      strcpy(status_disp, "Circling  ");
      break;
    case 4:
      strcpy(status_disp, "Stopped   ");
      break;
  }
  strcpy(display_line[DISP_0], status_disp);
  walltime2dec(time_disp);
  strcpy(display_line[DISP_1], time_disp);

  char ir_mov_status[DISP_TEXT_MAX];
  if (ir_status) {
    strcpy(ir_mov_status, "IR 1      ");
  } else {
    strcpy(ir_mov_status, "IR 0      ");
  }
  strcpy(display_line[DISP_3], ir_mov_status);

  update_lines();
}

void init_baud_rate_display(void) {
  set_clear_lines();
  strcpy(display_line[DISP_0], "Starting  ");
  strcpy(display_line[DISP_1], "Baud      ");
  strcpy(display_line[DISP_2], "Process   ");
  update_lines();
}

void show_baud_status(void) {
  set_clear_lines();
  strcpy(display_line[DISP_0], "Starting  ");
  strcpy(display_line[DISP_1], "Baud      ");
  strcpy(display_line[DISP_2], "Process   ");
  update_lines();
}

void display_baud(void) {
  unsigned int _swt = switch_press_time;
  unsigned int _wctc = wall_clock_time_count;

  set_clear_lines();

  // if (wall_clock_time_count < 25) {
  //   display_screen("Starting  ", "Baud      ", "Process   ", "",
  //   BOOLEAN_TRUE);
  // } else {
  set_line((char*)usb_char_rx, DISP_0, BOOLEAN_TRUE);
  display_screen("", EMPTY_STR, "   baud   ", NULL_STR, BOOLEAN_FALSE);
  if (baud_mode) {
    display_screen(NULL_STR, NULL_STR, NULL_STR, "  115200  ", BOOLEAN_FALSE);
  } else {
    display_screen(NULL_STR, NULL_STR, NULL_STR, "  460800  ", BOOLEAN_FALSE);
  }
  // }

  update_lines();
}

void set_line(char* line_str, int line_number, int should_clear_if_bad) {
  if ((line_str[0] != '\0') || (line_str[0] != 0))
    strcpy(display_line[line_number], line_str);
  else if (should_clear_if_bad) {
    strcpy(display_line[line_number], EMPTY_STR);
  }
}

void display_screen(char* l0, char* l1, char* l2, char* l3, int should_clear) {
  if (should_clear) {
    set_clear_lines();
  }
  set_line(l0, DISP_0, BOOLEAN_FALSE);
  set_line(l1, DISP_1, BOOLEAN_FALSE);
  set_line(l2, DISP_2, BOOLEAN_FALSE);
  set_line(l3, DISP_3, BOOLEAN_FALSE);
  update_lines();
}
