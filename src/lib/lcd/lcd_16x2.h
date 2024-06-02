#ifndef __LCD_16x2_H__
#define __LCD_16x2_H__

#include <pico/stdlib.h>

#define LCD_BUS 8

struct lcd_config {
  uint rs;
  uint en;
  uint data_bus[LCD_BUS];
};

void lcd_initialize_pins(struct lcd_config *lcd);
void lcd_enable_pulse(struct lcd_config *lcd);
void lcd_command(struct lcd_config *lcd, uint8_t cmd);
void lcd_data(struct lcd_config *lcd, uint8_t *data);
void lcd_initialize_8_bit_interface(struct lcd_config *lcd);
void lcd_goto_row2(struct lcd_config *lcd);

#endif //__LCD_16x2_H__

