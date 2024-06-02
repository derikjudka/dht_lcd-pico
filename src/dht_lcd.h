#ifndef __DHT_LCD_H__
#define __DHT_LCD_H__

#include <pico/stdlib.h>

struct dht_reading {
  float humidity;
  float temperature;
};

void host_start_signal();
void dht11_ack_signals();
int dht11_read(struct dht_reading *sensor_val);
float celsius_to_fahrenheit(float celsius);

#endif //__DHT_LCD_H__

