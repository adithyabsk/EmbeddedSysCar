/** @file config.h
 *  @brief Config header implementation
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef CONFIG_H
#define CONFIG_H

struct config_value {
  int min_val;
  int max_val;
  int curr_val;
};

void init_config(struct config_value*, int, int, int);
void update_config(struct config_value*, int);

#endif /* CONFIG_H */