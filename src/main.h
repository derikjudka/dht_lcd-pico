#ifndef __DHT_LCD_H__
#define __DHT_LCD_H__

#include <pico/stdlib.h>

struct dht_reading {
  float humidity;
  float temperature;
};

float celsius_to_fahrenheit(float celsius);

#endif //__DHT_LCD_H__

