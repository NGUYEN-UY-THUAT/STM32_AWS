/*
 * dht11.h
 *
 *  Created on: Feb 15, 2026
 *      Author: kukun
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"
#include <stdint.h>



typedef enum {
    DHT11_OK = 0,
    DHT11_ERROR,
    DHT11_TIMEOUT,
    DHT11_CHECKSUM_ERROR
} dht11_status_t;

typedef struct {
    uint8_t humidity;
    uint8_t temperature;
} dht11_data_t;

void dht11_init(void);

dht11_status_t dht11_read(dht11_data_t *data);

#endif

