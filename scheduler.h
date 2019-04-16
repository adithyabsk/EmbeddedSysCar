/** @file scheduler.c
 *  @brief Timing Scheduler defenition
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "common.h"

extern inline void init_scheduler(void);
unsigned int schedule_func_call(VOID_FUNC_PTR, int);
void run_scheduler(void);

#endif /* SCHEDULER_H */