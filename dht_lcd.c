#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint RS = 0;
const uint E = 1;
const uint D[8] = {2, 4, 6, 8, 11, 13, 14, 15};
const uint DHT_PIN = 16;

typedef struct {
    float humidity;
    float temperature;
} dht_reading;

void initialize_pins();
void enable_pulse();
void send_LCD(uint val);
void configure_LCD();
void write_LCD(uint val);
void second_row();
void prepare_dht_read();
void read_from_dht(dht_reading *reading);

int main() {
    initialize_pins();
    configure_LCD();

    float fahr;
    char *label = NULL, *info = NULL;

    label = "Temp  | Humidity";
    for (int i = 0; i < strlen(label); i++)
        write_LCD(label[i]);

    info = (char*) malloc(16*sizeof(char));
    dht_reading reading;
    sleep_ms(1000);
    read_from_dht(&reading);
    sleep_ms(2200);

    while (1) {
        read_from_dht(&reading);
        fahr = (reading.temperature * 9 / 5) + 32;
        snprintf(info, 16*sizeof(char), "%.1fF | %.1f%%", fahr, reading.humidity);
        second_row();
        for (int i = 0; i < strlen(info); i++) {
            write_LCD(info[i]);
        }
        sleep_ms(3000);
    }
    return 0;
}

void initialize_pins() {
    gpio_init(RS);
    gpio_init(E);
    gpio_set_dir(RS, GPIO_OUT);
    gpio_set_dir(E, GPIO_OUT);
    for (int i = 0; i < sizeof(D)/sizeof(D[0]); i++) {
        gpio_init(D[i]);
        gpio_set_dir(D[i], GPIO_OUT);
    }
    gpio_init(DHT_PIN);
}

void enable_pulse() {
    gpio_put(E, 1);
    sleep_ms(0.04);
    gpio_put(E, 0);
    sleep_ms(0.04);
}

void send_LCD(uint val) {
    gpio_put(D[0], ((val & 0b00000001) >> 0));
    gpio_put(D[1], ((val & 0b00000010) >> 1));
    gpio_put(D[2], ((val & 0b00000100) >> 2));
    gpio_put(D[3], ((val & 0b00001000) >> 3));
    gpio_put(D[4], ((val & 0b00010000) >> 4));
    gpio_put(D[5], ((val & 0b00100000) >> 5));
    gpio_put(D[6], ((val & 0b01000000) >> 6));
    gpio_put(D[7], ((val & 0b10000000) >> 7));
    enable_pulse();
}

void configure_LCD() {
    gpio_put(RS, 0);

    //necessary special case of 'Function set'
    send_LCD(0b00110000);
    sleep_ms(5);
    send_LCD(0b00110000);
    sleep_ms(1);
    send_LCD(0b00110000);
    sleep_ms(1);

    //configure 8-bit bus, 2 lines
    send_LCD(0b00111000);
    sleep_ms(1);

    //display on, cursor off, blink off
    send_LCD(0b00001100);
    sleep_ms(1);

    //entry mode set
    send_LCD(0b00000100);
    sleep_ms(1);

    //clear screen
    send_LCD(0b00000001);
    sleep_ms(5);
}

void write_LCD(uint val) {
    gpio_put(RS, 1);
    send_LCD(val);
    sleep_ms(5);
}

void second_row() {
    gpio_put(RS, 0);
    send_LCD(0b10101001);
    sleep_ms(1);
}

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

void read_from_dht(dht_reading *reading) {
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
        reading->humidity = data[0] + (data[1] / 255.0);
        reading->temperature = (data[2] + ((data[3] & 0x7F) / 255.0));
        if (data[3] & 0x80) {
            reading->temperature *= -1;
        }   
    }
}