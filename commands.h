/** @file commands.h
 *  @brief Serial port configuration
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef CMD_H
#define CMD_H

#include "display.h"

#ifndef CMD_LOCAL_DEF
#define CMD_LOCAL_DEF extern
#endif

#define CMD_QUEUE_SIZE (10)
#define CMD_STR_SIZE (11)

CMD_LOCAL_DEF char most_recent_cmd[DISP_TEXT_SIZE];

extern inline void init_command_processor(void);

void process_cmd_queue(void);

void insert_cmd(char[]);

void search_cmds(void);

#endif /* CMD_H */