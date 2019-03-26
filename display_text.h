/** @file display_text.h
 *  @brief Display function prototype declarations
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DISPLAY_TEXT_H
#define DISPLAY_TEXT_H

// External LCD Functions
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

/**
 * @brief Initializes the display variables
 * @note This must be called before any of the other functions
 * can be called.
 *
 */
void init_display(void);

/**
 * @brief Displays a clear screen
 *
 */
void clear_display(void);

/**
 * @brief Displays initial screen
 *
 */
void reset_display(void);

/**
 * @brief Displays the status of both the ADC variables,
 * the IR LED, and a logicla decision whether or not the
 * car is on the left or right side of a black line
 *
 */
void show_adc_status(void);

void show_line_follow_status(void);

#endif /* DISPLAY_TEXT_H */