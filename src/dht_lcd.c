#include "dht_lcd.h"
#include "lcd_16x2.h"

#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define USE_FAHR 1

#define LCD_NUM_COLUMN 16

#define DHT_PIN 17

#define DHT11_XFER_BYTES 5

#define SIGN_BIT_MASK 0x80
#define TEMPERATURE_DEC_MASK 0x7F

#define DHT11_SLEEP_TIME_MS 1000

struct lcd_config lcd = {
    .rs = 0,
    .en = 1,
    .data_bus = {2, 4, 6, 8, 10, 12, 14,
                 15} // modify to match connected gpios on raspberry pi pico
};

int main() {
  gpio_init(DHT_PIN);
  lcd_initialize_pins(&lcd);
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

void host_start_signal() {
  gpio_set_dir(DHT_PIN, GPIO_OUT);
  gpio_put(DHT_PIN, 0);
  sleep_ms(20);
  gpio_set_dir(DHT_PIN, GPIO_IN);
  sleep_us(20);
}

void dht11_ack_signals() {
  while (gpio_get(DHT_PIN) == 0);
  while (gpio_get(DHT_PIN) == 1);
}

int dht11_read(struct dht_reading *sensor_read) {
  uint8_t data[DHT11_XFER_BYTES] = {0};

  host_start_signal();
  dht11_ack_signals();

  for (int i = 0; i < DHT11_XFER_BYTES * 8; i++) {
    while (gpio_get(DHT_PIN) == 0); // wait for the start of the data bit
    sleep_us(30);
    if (gpio_get(DHT_PIN) == 1) {
      data[i / 8] <<= 1;
      data[i / 8] |= 1;
      while (gpio_get(DHT_PIN) == 1); // wait for the end of data bit
    } else {
      data[i / 8] <<= 1;
    }
  }

  if ((data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))) {
    sensor_read->humidity = data[0] + (data[1] * 0.1);
    sensor_read->temperature =
        data[2] + ((data[3] & TEMPERATURE_DEC_MASK) * 0.1);
    if (data[3] & SIGN_BIT_MASK)
      sensor_read->temperature *= -1;
    return 0;
  } else {
    return -1;
  }
}

float celsius_to_fahrenheit(float celsius) { return ((celsius * 9 / 5) + 32); }

