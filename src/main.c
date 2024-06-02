#include "main.h"
#include "dht.h"
#include "lcd_16x2.h"

#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define USE_FAHR 1

#define LCD_NUM_COLUMN 16
#define DHT_PIN 17
#define DHT11_SLEEP_TIME_MS 1000

struct lcd_config lcd = {
    .rs = 0,
    .en = 1,
    .data_bus = {2, 4, 6, 8, 10, 12, 14,
                 15} // modify to match connected gpios on raspberry pi pico
};

int main() {
  // Initialize GPIO
  gpio_init(DHT_PIN);
  lcd_initialize_pins(&lcd);

  // Initialize LCD display
  lcd_initialize_8_bit_interface(&lcd);

  char *label = "Temp  | Humidity";
  char *info = (char *)malloc(LCD_NUM_COLUMN * sizeof(char));

  struct dht_reading *sensor_read =
      (struct dht_reading *)malloc(sizeof(struct dht_reading));
  for (int i = 0; i < strlen(label); i++)
    lcd_data(&lcd, (uint8_t *)&label[i]);

  sleep_ms(1000);
  while (1) {
    if (!dht11_read(sensor_read)) {

#ifdef USE_FAHR
      snprintf(info, LCD_NUM_COLUMN * sizeof(char), "%.1fF | %.1f%%",
               celsius_to_fahrenheit(sensor_read->temperature),
               sensor_read->humidity);
#else
      snprintf(info, LCD_NUM_COLUMN * sizeof(char), "%.1fC | %.1f%%",
               sensor_read->temperature, sensor_read->humidity);
#endif

      lcd_goto_row2(&lcd);
      for (int i = 0; i < strlen(info); i++) {
        lcd_data(&lcd, (uint8_t *)&info[i]);
      }
    }
    sleep_ms(DHT11_SLEEP_TIME_MS);
  }

  return 0;
}

float celsius_to_fahrenheit(float celsius) { return ((celsius * 9 / 5) + 32); }

