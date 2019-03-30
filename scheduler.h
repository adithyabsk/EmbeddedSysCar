/** @file scheduler.c
 *  @brief Timing Scheduler defenition
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef void (*VOID_FUNC_PTR)(void);

void init_scheduler(void);
int schedule_func_call(VOID_FUNC_PTR, int);
void run_scheduler(void);

#endif /* SCHEDULER_H */