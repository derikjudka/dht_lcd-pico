#ifndef __DHT_H__
#define __DHT_H__

#include "main.h"

void host_start_signal();
void dht11_ack_signals();
int dht11_read(struct dht_reading *sensor_val);

#endif //__DHT_H__

