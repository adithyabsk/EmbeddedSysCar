//------------------------------------------------------------------------------
//
//  Description: This file controls the car driving different 
//
//
//  Adithya Balaji
//  Feb 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (7.12.1)
//------------------------------------------------------------------------------

#include "functions.h"
#include "macros.h"
#include "msp430.h"

// Main time vars
extern unsigned int Last_Time_Sequence;
extern unsigned int cycle_time;
extern unsigned int time_change;
extern unsigned int normal_time;

// Shape selection
unsigned char selected_shape;
volatile unsigned char run_state;
unsigned int f_eight_loop;
unsigned int triangle_loop;

// Drive Vars
unsigned int delay_start;
unsigned int right_motor_count;
unsigned int left_motor_count;
unsigned int segment_count;

void default_shape_setup(void) {
  selected_shape = CIRCLE;
  run_state = NONE;
  f_eight_loop = INIT_STATE_ZERO;
}

void cycle_shapes(void) {
  static unsigned int state = INIT_STATE_ZERO;
  if (state % SHAPE_COUNT == STRAIGHT_ENUM) {
    selected_shape = STRAIGHT;
  } else if (state % SHAPE_COUNT == FIGURE_EIGHT_ENUM) {
    selected_shape = FIGURE_EIGHT;
  } else if (state % SHAPE_COUNT == TRIANGLE_ENUM) {
    selected_shape = TRIANGLE;
  } else {
    selected_shape = CIRCLE;
  }
  state++;
  show_shapes_menu(selected_shape);
}

void process_shapes(void) {
  switch (selected_shape) {
    case STRAIGHT:
      run_straight();
      break;
    case FIGURE_EIGHT:
      run_figure_eight();
      break;
    case TRIANGLE:
      run_triangle();
      break;
    case CIRCLE:
      run_circle();
      break;
    default:
      break;
  }
}

void run_straight(void) {
  switch (run_state) {
    case WAIT:
      wait_case();
      break;
    case START:
      start_case();
      break;
    case RUN:
      run_case_params(10,  // segment_count
                      2,   // left_count_time 2 straight
                      9,   // right_count_time 9 straight
                      10   // period
      );
      break;
    case END:
      end_case();
      break;
    default:
      break;
  }
}

void run_figure_eight(void) {
  switch (run_state) {
    case WAIT:
      wait_case();
      break;
    case START:
      start_case();
      break;
    case RUN:
      if (f_eight_loop == 0) {
        run_case_params(16,  // segment_count
                        50,  // left_count_time 2 straight
                        2,   // right_count_time 9 straight
                        50   // period
        );
      } else if (f_eight_loop % 2 == 0) {
        run_case_params(15,  // segment_count
                        50,  // left_count_time 2 straight
                        2,   // right_count_time 9 straight
                        50   // period
        );
      } else {
        run_case_params(15,  // segment_count
                        2,   // left_count_time 2 straight
                        50,  // right_count_time 9 straight
                        50   // period
        );
      }
      break;
    case END:
      end_case();
      break;
    default:
      break;
  }
}

void run_triangle(void) {
  switch (run_state) {
    case WAIT:
      wait_case();
      break;
    case START:
      start_case();
      break;
    case RUN:
      if (triangle_loop == 0) {
        run_case_params(15,  // segment_count
                        2,   // left_count_time 2 straight
                        9,   // right_count_time 9 straight
                        10   // period
        );
      } else if (triangle_loop % 2 == 0) {
        run_case_params(16,  // segment_count
                        2,   // left_count_time 2 straight
                        9,   // right_count_time 9 straight
                        10   // period
        );
      } else {
        run_case_params(1,    // segment_count
                        100,  // left_count_time 2 straight
                        0,    // right_count_time 9 straight
                        100   // period
        );
      }
      break;
    case END:
      end_case();
      break;
    default:
      break;
  }
}

void run_circle(void) {
  switch (run_state) {
    case WAIT:
      wait_case();
      break;
    case START:
      start_case();
      break;
    case RUN:
      run_case_params(27,  // segment_count
                      50,  // left_count_time 2 straight
                      2,   // right_count_time 9 straight
                      50   // period
      );
      break;
    case END:
      end_case();
      break;
    default:
      break;
  }
}

void wait_case(void) {
  if (time_change) {
    time_change = INIT_STATE_ZERO;
    if (delay_start++ >= WAITING2START) {
      delay_start = INIT_STATE_ZERO;
      run_state = START;
    }
  }
}

void start_case(void) {
  cycle_time = INIT_STATE_ZERO;
  right_motor_count = INIT_STATE_ZERO;
  left_motor_count = INIT_STATE_ZERO;
  forward_on();
  segment_count = INIT_STATE_ZERO;
  run_state = RUN;
}

void run_case_params(int travel_distance, int left_count_time,
                     int right_count_time, int wheel_count_time) {
  if (time_change) {
    time_change = INIT_STATE_ZERO;
    if (segment_count <= travel_distance) {
      if (left_motor_count++ >= left_count_time) {
        L_DRIVE_OFF;
      }
      if (right_motor_count++ >= right_count_time) {
        R_DRIVE_OFF;
      }
      if (cycle_time >= wheel_count_time) {
        cycle_time = INIT_STATE_ZERO;
        right_motor_count = INIT_STATE_ZERO;
        left_motor_count = INIT_STATE_ZERO;
        segment_count++;
        forward_on();
      }
    } else {
      if ((selected_shape == FIGURE_EIGHT) && (f_eight_loop < 3)) {
        run_state = START;
        f_eight_loop++;
      } else if ((selected_shape == TRIANGLE) && (triangle_loop < 11)) {
        run_state = START;
        triangle_loop++;
      } else {
        triangle_loop = INIT_STATE_ZERO;
        f_eight_loop = INIT_STATE_ZERO;
        run_state = END;
      }
    }
  }
}

void end_case(void) {
  forward_off();
  run_state = NONE;
}

void forward_on(void) {
  R_DRIVE_ON;
  L_DRIVE_ON;
}

void forward_off(void) {
  R_DRIVE_OFF;
  L_DRIVE_OFF;
}