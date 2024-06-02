#include "dht.h"

#include <pico/stdlib.h>

#define SIGN_BIT_MASK 0x80
#define TEMPERATURE_DEC_MASK 0x7F

#define DHT_PIN 17
#define DHT11_XFER_BYTES 5

void host_start_signal() {
  gpio_set_dir(DHT_PIN, GPIO_OUT);
  gpio_put(DHT_PIN, 0);
  sleep_ms(20);
  gpio_set_dir(DHT_PIN, GPIO_IN);
  sleep_us(20);
}

void dht11_ack_signals() {
  while (gpio_get(DHT_PIN) == 0)
    ;
  while (gpio_get(DHT_PIN) == 1)
    ;
}

int dht11_read(struct dht_reading *sensor_read) {
  uint8_t data[DHT11_XFER_BYTES] = {0};

  host_start_signal();
  dht11_ack_signals();

  for (int i = 0; i < DHT11_XFER_BYTES * 8; i++) {
    while (gpio_get(DHT_PIN) == 0)
      ; // wait for the start of the data bit
    sleep_us(30);
    if (gpio_get(DHT_PIN) == 1) {
      data[i / 8] <<= 1;
      data[i / 8] |= 1;
      while (gpio_get(DHT_PIN) == 1)
        ; // wait for the end of data bit
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

