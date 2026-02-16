/*
 * sensor_util.c
 *
 *  Created on: Feb 15, 2026
 *      Author: kukun
 */
#include "sensor_logging_config.h"
#include "sensor_util.h"
#include "dht11.h"
#include "logging_stack.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sensor_logging_config.h"


static dht11_data_t cached_data;
static uint32_t last_read_tick = 0;
static uint8_t initialized = 0;

static void update_sensor_if_needed(void)
{
    uint32_t now = xTaskGetTickCount();

    if ((now - last_read_tick) < pdMS_TO_TICKS(1000))
        return;

    taskENTER_CRITICAL();

    for (int i = 0; i < 3; i++)
    {
        if (dht11_read(&cached_data) == DHT11_OK)
        {
            last_read_tick = now;
            taskEXIT_CRITICAL();
            return;
        }
    }

    taskEXIT_CRITICAL();
}

int init_temperature_humidity_sensor(void)
{
    LogInfo(("Initializing DHT11 on PA4..."));

    dht11_init();

    initialized = 1;
    last_read_tick = 0;

    LogInfo(("DHT11 ready."));

    return 0;
}

void get_temperature_reading(float *temperature)
{
    if (!initialized || temperature == NULL)
        return;

    update_sensor_if_needed();

    *temperature = (float)cached_data.temperature;

    LogInfo(("Temperature: %.2f C", *temperature));
}

void get_humidity_reading(uint8_t *humidity)
{
    if (!initialized || humidity == NULL)
        return;

    update_sensor_if_needed();

    *humidity = cached_data.humidity;

    LogInfo(("Humidity: %u %%", *humidity));
}

