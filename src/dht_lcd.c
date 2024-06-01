#include "dht_lcd.h"
#include "lcd_16x2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define USE_FAHR 1

#define DHT_PIN 16

#define LCD_NUM_COLUMN 16
#define LCD_NUM_ROW 2

struct lcd_config lcd = {
    .rs = 0,
    .en = 1,
    .data_bus = {2, 4, 6, 8, 10, 12, 14,
                 15} // modify to match connected gpios on raspberry pi pico
};

int main() {
  lcd_initialize_pins(&lcd);
  gpio_init(DHT_PIN);
  lcd_initialize_8_bit_interface(&lcd);

  char *label = "Temp  | Humidity";
  char *info = NULL;

  struct dht_reading *sensor_val =
      (struct dht_reading *)malloc(sizeof(struct dht_reading));

  for (int i = 0; i < strlen(label); i++)
    lcd_data(&lcd, (uint8_t *)&label[i]);

  info = (char *)malloc(LCD_NUM_COLUMN * sizeof(char));
  sleep_ms(1000);
  sensor_val->humidity = 33;    // test value
  sensor_val->temperature = 10; // test_value
  sleep_ms(2200);

  while (1) {
    sensor_val->temperature += 0.1;
#ifdef USE_FAHR
    snprintf(info, LCD_NUM_COLUMN * sizeof(char), "%.1fF | %.1f%%",
             celsius_to_fahrenheit(sensor_val->temperature),
             sensor_val->humidity);
#else
    snprintf(info, LCD_NUM_COLUMN * sizeof(char), "%.1fC | %.1f%%",
             sensor_val->temperature, sensor_val->humidity);
#endif
    lcd_goto_row2(&lcd);
    for (int i = 0; i < strlen(info); i++) {
      lcd_data(&lcd, (uint8_t *)&info[i]);
    }
    sleep_ms(1000);
  }

  return 0;
}

/*
void prepare_dht_read() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 1);
    sleep_ms(1000);
    gpio_put(DHT_PIN, 0);
    sleep_ms(20); //MCU pulls low to signal request for data from DHT
    gpio_set_dir(DHT_PIN, GPIO_IN);
    //next 3 lines are waiting for signals sent by DHT data pin
    sleep_us(15);
    sleep_us(83);
    sleep_us(88);
}

void read_from_dht(struct dht_reading *sensor_val) {
    uint data[5] = {0, 0, 0, 0, 0};
    prepare_dht_read();
    for (int i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
        uint signal = 0;
        for (int j = 0; j < 8; j++) {
            while (!gpio_get(DHT_PIN));
            sleep_us(30);
            if (gpio_get(DHT_PIN)) {
                signal = (signal << 1) | 1;
                while(gpio_get(DHT_PIN));
            }
            else {
                signal <<= 1;
            }
        }
        data[i] = signal;
    }
    if (data[4] == data[0] + data[1] + data[2] + data[3]) {
        sensor_val->humidity = data[0] + (data[1] / 255.0);
        sensor_val->temperature = (data[2] + ((data[3] & 0x7F) / 255.0));
        if (data[3] & 0x80) {
            sensor_val->temperature *= -1;
        }
    }
}
*/

float celsius_to_fahrenheit(float celsius) { return ((celsius * 9 / 5) + 32); }
