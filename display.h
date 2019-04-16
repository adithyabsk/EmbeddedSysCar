/** @file display.h
 *  @brief Raw display functionality interface
 *
 *  @author Adithya Balaji (adithyabsk)
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#define DISP_MAX_ROWS (4)
#define DISP_TEXT_MAX (11)
#define EMPTY_STR ("          ")
#define NULL_STR ("")
#define NULL_CHAR ('\0')

/*
 * External LCD Functions
 */

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

/*
 * End External LCD Functions
 */

// Custom LCD functions

/**
 * @brief Initializes the display variables
 * @note This must be called before any of the other functions
 * can be called.
 *
 */
extern inline void init_display(void);

/**
 * @brief Displays a clear screen
 *
 */
void update_lines(void);

void set_line(const char *, int, int);
void display_screen(const char *, const char *, const char *, const char *,
                    int);

#endif /* DISPLAY_H */