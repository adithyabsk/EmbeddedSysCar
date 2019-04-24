/** @file commands.c
 *  @brief Implementation of command stream processing
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#define CMD_LOCAL_DEF
#include "commands.h"

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "display_text.h"
#include "drive.h"
#include "serial.h"

#define CMD_LOOKUP_COUNT (6)

enum cmd_search_state { SS_NONE, SS_INTERCEPTING };

struct cmd_buff {
  char cmd[CMD_STR_SIZE];
  unsigned int cmd_wr_idx;
};

struct cmd_lookup_item {
  char cmd[CMD_STR_SIZE];
  VOID_FUNC_PTR func;
};

struct cmd_queue_type {
  char cmds[CMD_QUEUE_SIZE][CMD_STR_SIZE];
  unsigned int rd_idx;
  unsigned int wr_idx;
};

struct cmd_queue_type cmd_queue;
struct cmd_buff iot_cmd_buff;
struct cmd_buff usb_cmd_buff;
enum cmd_search_state iot_cmd_search_state;
enum cmd_search_state usb_cmd_search_state;

static struct cmd_lookup_item cmd_lookup_table[CMD_LOOKUP_COUNT] = {
    {"$w\r", &set_drive_forward}, {"$a\r", &set_drive_left},
    {"$s\r", &set_drive_reverse}, {"$d\r", &set_drive_right},
    {"$p\r", &set_drive_stop},
    // {"$+\r", &increment_checkpoint},
    // {"$*\r", &toggle_auto},
};

static void lookup_cmd(char[CMD_STR_SIZE]);
static int load_cmd_buff(char[SERIAL_RING_SIZE], struct cmd_buff*);
static void init_cmd_buffer(struct cmd_buff*);
static void init_cmd_queue(void);

inline void init_command_processor(void) {
  init_cmd_queue();
  init_cmd_buffer(&iot_cmd_buff);
  init_cmd_buffer(&usb_cmd_buff);
  iot_cmd_search_state = SS_NONE;
  usb_cmd_search_state = SS_NONE;
  strcpy(most_recent_cmd, EMPTY_STR);
}

static void init_cmd_buffer(struct cmd_buff* buff) {
  memset(buff->cmd, NULL_CHAR, CMD_STR_SIZE);
  buff->cmd_wr_idx = INIT_CLEAR;
}

static void init_cmd_queue(void) {
  cmd_queue.rd_idx = INIT_CLEAR;
  cmd_queue.wr_idx = INIT_CLEAR;
  int i;
  for (i = INIT_CLEAR; i < CMD_QUEUE_SIZE; i++) {
    memset(cmd_queue.cmds[i], NULL_CHAR, CMD_STR_SIZE);
  }
}

void insert_cmd(char new_cmd[CMD_STR_SIZE]) {
  memset(cmd_queue.cmds[cmd_queue.wr_idx], NULL_CHAR, CMD_STR_SIZE);
  strcpy(cmd_queue.cmds[cmd_queue.wr_idx++], new_cmd);
  if (cmd_queue.wr_idx >= CMD_QUEUE_SIZE) {
    cmd_queue.wr_idx = INIT_CLEAR;
  }
}

void process_cmd_queue(void) {
  if (cmd_queue.rd_idx != cmd_queue.wr_idx) {
    int i;
    for (i = INIT_CLEAR; i < abs(cmd_queue.rd_idx - cmd_queue.wr_idx); i++) {
      lookup_cmd(cmd_queue.cmds[(cmd_queue.rd_idx + i) % CMD_QUEUE_SIZE]);
      truncate_disp_str(
          most_recent_cmd,
          cmd_queue.cmds[(cmd_queue.rd_idx + i) % CMD_QUEUE_SIZE]);
    }
    cmd_queue.rd_idx = cmd_queue.wr_idx;
  }
}

void cmd_search_fsm(enum cmd_search_state* css,
                    char search_buff[SERIAL_RING_SIZE], char cmd_start_char,
                    struct cmd_buff* sel_cmd_buff) {
  char* cmd_end_ptr = strchr(search_buff, '\r');
  char* cmd_start_ptr = strchr(search_buff, cmd_start_char);
  char partial_cmd[SERIAL_RING_SIZE];

  int test_len = strlen(search_buff);

  switch (*css) {
    case SS_NONE:
      if (cmd_start_ptr) {
        if (cmd_end_ptr) {
          // full command in buff
          // bypass command buffer struct
          int cmd_len = pointerdiff2len(cmd_end_ptr - cmd_start_ptr);
          if (cmd_len <= max_str_len(CMD_STR_SIZE)) {
            char temp_cmd[CMD_STR_SIZE];
            memset(temp_cmd, NULL_CHAR, CMD_STR_SIZE);
            strncpy(temp_cmd, cmd_start_ptr, cmd_len);
            insert_cmd(temp_cmd);
          }
        } else {
          // partial command in buff
          memset(partial_cmd, NULL_CHAR, SERIAL_RING_SIZE);
          strcpy(partial_cmd, cmd_start_ptr);
          if (load_cmd_buff(partial_cmd, sel_cmd_buff)) {
            *css = SS_INTERCEPTING;
          }
        }
      }
      break;
    case SS_INTERCEPTING:
      memset(partial_cmd, NULL_CHAR, SERIAL_RING_SIZE);
      if (cmd_end_ptr) {
        strncpy(partial_cmd, search_buff,
                pointerdiff2len(cmd_end_ptr - search_buff));
        if (load_cmd_buff(partial_cmd, sel_cmd_buff)) {
          insert_cmd(sel_cmd_buff->cmd);
          init_cmd_buffer(sel_cmd_buff);
        }
        *css = SS_NONE;
      } else {
        strcpy(partial_cmd, search_buff);
        if (!load_cmd_buff(partial_cmd, sel_cmd_buff)) {
          *css = SS_NONE;
          init_cmd_buffer(sel_cmd_buff);
        }
      }
      break;
    default:
      break;
  }
}

static int load_cmd_buff(char cmd_substr[SERIAL_RING_SIZE],
                         struct cmd_buff* buff) {
  int cmd_substr_len = strlen(cmd_substr);
  if (cmd_substr_len <= CMD_STR_SIZE - buff->cmd_wr_idx) {
    strcpy((char*)(buff->cmd + buff->cmd_wr_idx), cmd_substr);
    buff->cmd_wr_idx = cmd_substr_len;
    return BOOLEAN_TRUE;
  } else {
    memset(buff->cmd, NULL_CHAR,
           CMD_STR_SIZE);  // clear command (cmd is too long)
    return BOOLEAN_FALSE;
  }
}

static void lookup_cmd(char input_cmd[CMD_STR_SIZE]) {
  int i;
  for (i = INIT_CLEAR; i < CMD_LOOKUP_COUNT; i++) {
    if (!strcmp(cmd_lookup_table[i].cmd, input_cmd)) {
      (*(cmd_lookup_table[i].func))();
    }
  }
}

void search_cmds(void) {
  char iot_unread[SERIAL_RING_SIZE];
  char usb_unread[SERIAL_RING_SIZE];

  if (read_buff(&iot_ring_buff, iot_unread)) {
    cmd_search_fsm(&iot_cmd_search_state, iot_unread, '$', &iot_cmd_buff);
  }
  if (read_buff(&usb_ring_buff, usb_unread)) {
    cmd_search_fsm(&usb_cmd_search_state, usb_unread, '^', &usb_cmd_buff);
  }
}