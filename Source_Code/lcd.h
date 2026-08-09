#ifndef LCD_H
#define LCD_H

void lcd_init();
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_print(char *str);

#endif
