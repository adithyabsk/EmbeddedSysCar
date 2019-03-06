//******************************************************************************
//
//  Description: This file contains the Function prototypes
//
//  Adithya Balaji
//  Jan 2019
//  Built with IAR Embedded Workbench Version: V4.10A/W32 (5.40.1)
//  Initial version by Jim Carlson
//******************************************************************************
// Functions

// Initialization
void Init_Conditions(void);

// Interrupts
void enable_interrupts(void);
__interrupt void Timer0_B0_ISR(void);
__interrupt void switch_interrupt(void);

// Analog to Digital Converter

// Clocks
void Init_Clocks(void);
void Reset_SMClock8MHz(void);
void Set_SMClock500kHz(void);

// LED Configurations
void Init_LEDs(void);
void IR_LED_control(char selection);
void Backlite_control(char selection);

// LCD
void Display_Process(void);
void Display_Update(char p_L1, char p_L2, char p_L3, char p_L4);
void enable_display_update(void);
void update_string(char *string_data, int string);
void Init_LCD(void);
void lcd_clear(void);
void lcd_putc(char c);
void lcd_puts(char *s);

void lcd_power_on(void);
void lcd_write_line1(void);
void lcd_write_line2(void);
// void lcd_draw_time_page(void);
// void lcd_power_off(void);
void lcd_enter_sleep(void);
void lcd_exit_sleep(void);
// void lcd_write(unsigned char c);
// void out_lcd(unsigned char c);

void Write_LCD_Ins(char instruction);
void Write_LCD_Data(char data);
void ClrDisplay(void);
void ClrDisplay_Buffer_0(void);
void ClrDisplay_Buffer_1(void);
void ClrDisplay_Buffer_2(void);
void ClrDisplay_Buffer_3(void);

void SetPostion(char pos);
void DisplayOnOff(char data);
void lcd_BIG_mid(void);
void lcd_4line(void);
void lcd_out(char *s, char line, char position);
void lcd_rotate(char view);

// void lcd_write(char data, char command);
void lcd_write(unsigned char c);
void lcd_write_line1(void);
void lcd_write_line2(void);
void lcd_write_line3(void);

void lcd_command(char data);
void LCD_test(void);
void LCD_iot_meassage_print(int nema_index);

// Menu
void Menu_Process(void);

// Ports
void Init_Ports(void);
void Init_Ports_1(void);
void Init_Ports_2(void);
void Init_Ports_3(int);
void Init_Ports_4(void);
void Init_Ports_5(void);
void Init_Ports_6(void);

// SPI
void Init_SPI_B1(void);
void SPI_B1_write(char byte);
void spi_rs_data(void);
void spi_rs_command(void);
void spi_LCD_idle(void);
void spi_LCD_active(void);
void SPI_test(void);
void WriteIns(char instruction);
void WriteData(char data);

// Switches
void Init_Switches(void);
void switch_control(void);
void enable_switch_SW1(void);
void enable_switch_SW2(void);
void disable_switch_SW1(void);
void disable_switch_SW2(void);
void Switches_Process(void);
void Init_Switch(void);
void Switch_Process(void);
void Switch1_Process(void);
void Switch2_Process(void);
void menu_act(void);
void menu_select(void);

// Timers
void Init_Timers(void);
void Init_Timer_B0(void);
void Init_Timer_B1(void);
void Init_Timer_B2(void);
void Init_Timer_B3(void);

void usleep(unsigned int usec);
void usleep10(unsigned int usec);
void five_msec_sleep(unsigned int msec);
void measure_delay(void);
void out_control_words(void);

// Display
void clear_display(void);
void reset_display(void);
void show_shapes_menu(char c);
void set_clear_lines(void);
void update_lines(void);
void show_button_status(void);
void show_switch(char sw_status);
void show_fr_run_status(void);
void show_adc_status(void);

// Custom Switches
void cycle_shapes(void);
void set_switch_states(void);
void default_shape_setup(void);
void process_shapes(void);
void run_for_rev(void);
void run_straight(void);
void run_figure_eight(void);
void run_triangle(void);
void run_circle(void);
void wait_case(void);
void start_case(void);
void set_run(void);
void run_case(void);
void run_case_params(int travel_distance, int left_count_time,
                     int right_count_time, int wheel_count_time);
void end_case(void);
void forward_on(void);
void forward_off(void);

// Drive
void stop_drive(void);
void drive_forward(void);
void drive_reverse(void);
void drive_cw(void);
void drive_ccw(void);

// Init_adc
void init_adc(void);