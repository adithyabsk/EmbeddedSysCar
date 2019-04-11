/** @file llist.c
 *  @brief Implementation of timing llist
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "scheduler.h"

#include <string.h>

#include "common.h"
#include "timers.h"

#define SCHED_SIZE (10)

struct sched_entry {
  VOID_FUNC_PTR func;
  unsigned int call_time;
  unsigned int wait_time;
  unsigned int completed;
};

unsigned int sched_pos = INIT_CLEAR;
static struct sched_entry scheduler[SCHED_SIZE];

void assign_schedule_item(VOID_FUNC_PTR f, int wt, int pos) {
  unsigned int curr_time = wall_clock_time_count;
  scheduler[pos].func = f;
  scheduler[pos].call_time = curr_time;
  scheduler[pos].wait_time = wt;
  scheduler[pos].completed = INIT_CLEAR;
}

int schedule_func_call(VOID_FUNC_PTR f, int wt) {
  int i;
  for (i = INIT_CLEAR; i < SCHED_SIZE; i++) {
    if (scheduler[i].completed) {
      assign_schedule_item(f, wt, i);
      return BOOLEAN_TRUE;
    }
  }
  return BOOLEAN_FALSE;
}

void init_scheduler(void) {
  memset(&scheduler, INIT_CLEAR, sizeof(scheduler));
  int i;
  for (i = INIT_CLEAR; i < SCHED_SIZE; i++) {
    scheduler[i].completed = BOOLEAN_TRUE;
  }
}

void run_scheduler(void) {
  unsigned int curr_time = wall_clock_time_count;

  int i;
  for (i = 0; i < SCHED_SIZE; i++) {
    if ((curr_time > (scheduler[i].call_time + scheduler[i].wait_time)) &&
        (scheduler[i].func) && (scheduler[i].completed == 0)) {
      (*(scheduler[i].func))();
      scheduler[i].completed = BOOLEAN_TRUE;
    }
  }
}