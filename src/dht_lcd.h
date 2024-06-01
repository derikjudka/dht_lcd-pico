#ifndef __DHT_LCD_H__
#define __DHT_LCD_H__

#include <pico/stdlib.h>

struct dht_reading {
  float humidity;
  float temperature;
};

// void prepare_dht_read();
// void read_from_dht(struct dht_reading *sensor_val);
float celsius_to_fahrenheit(float celsius);

#endif //__DHT_LCD_H__

