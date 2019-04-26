/** @file config.c
 *  @brief Dynamic configuration updater implementation
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#include "config.h"

void init_config(struct config_value* cv, int min_val, int max_val,
                 int default_val) {
  cv->min_val = min_val;
  cv->max_val = max_val;
  cv->curr_val = default_val;
}

void update_config(struct config_value* cv, int new_val) {
  if ((new_val >= cv->min_val) && (new_val <= cv->max_val)) {
    cv->curr_val = new_val;
  }
}