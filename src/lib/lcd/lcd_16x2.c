#include "lcd_16x2.h"

#define CLEAR_DISPLAY (0x1 << 0)
#define RETURN_HOME (0x1 << 1)

#define ENTRY_MODE_SET (0x1 << 2)
#define CURSOR_INC (0x1 << 1)
#define EN_DISP_SHIFT (0x1 << 0)

#define DISPLAY_CTRL (0x1 << 3)
#define DISPLAY_ON (0x1 << 2)
#define CURSOR_ON (0x1 << 1)
#define CURSOR_BLINK (0x1 << 0)

#define FUNC_SET (0x1 << 5)

#define DATA_LEN_8 (0x1 << 4)
#define DATA_LEN_4 (0x0 << 4)

#define NUM_LINE_2 (0x1 << 3)
#define NUM_LINE_1 (0x0 << 3)

#define FONT_TYPE_5_11 (0x1 << 2)
#define FONT_TYPE_5_8 (0x0 << 2)

#define SET_CGRAM (0x1 << 6)
#define SET_DDRAM (0x1 << 7)

#define READ_BUSY_FLAG (0x1 << 8)

#define ROW_2_START_ADDR 0x40

void lcd_initialize_pins(struct lcd_config *lcd) {
  gpio_init(lcd->rs);
  gpio_set_dir(lcd->rs, GPIO_OUT);

  gpio_init(lcd->en);
  gpio_set_dir(lcd->en, GPIO_OUT);

  for (int i = 0; i < LCD_BUS; i++) {
    gpio_init(lcd->data_bus[i]);
    gpio_set_dir(lcd->data_bus[i], GPIO_OUT);
  }
}

void lcd_enable_pulse(struct lcd_config *lcd) {
  gpio_put(lcd->en, 1);
  sleep_us(1);
  gpio_put(lcd->en, 0);
  sleep_us(50);
}

void lcd_command(struct lcd_config *lcd, uint8_t cmd) {
  gpio_put(lcd->rs, 0); // Command mode

  for (int i = 0; i < LCD_BUS; i++) {
    gpio_put(lcd->data_bus[i], (cmd >> i) & 0x1);
  }
  lcd_enable_pulse(lcd);
}

void lcd_data(struct lcd_config *lcd, uint8_t *data) {
  gpio_put(lcd->rs, 1); // Data mode

  for (int i = 0; i < LCD_BUS; i++) {
    gpio_put(lcd->data_bus[i], (*data >> i) & 0x1);
  }
  lcd_enable_pulse(lcd);
}

void lcd_initialize_8_bit_interface(struct lcd_config *lcd) {
  // wait for Vdd to rise to 4.5V
  sleep_ms(50);

  // function set
  lcd_command(lcd, (FUNC_SET | DATA_LEN_8 | NUM_LINE_2));
  sleep_ms(5); // wait for command to process

  // display control
  lcd_command(lcd, (DISPLAY_CTRL | DISPLAY_ON));
  sleep_ms(5); // wait for command to process

  // display clear
  lcd_command(lcd, CLEAR_DISPLAY);
  sleep_ms(5); // wait for command to process

  // entry mode set
  lcd_command(lcd, (ENTRY_MODE_SET | CURSOR_INC));
  sleep_ms(5); // wait for command to process
}

void lcd_goto_row2(struct lcd_config *lcd) {
  lcd_command(lcd, (SET_DDRAM | ROW_2_START_ADDR));
}

