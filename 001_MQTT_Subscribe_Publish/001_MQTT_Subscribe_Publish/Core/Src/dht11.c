/*
 * dht11.c
 *
 *  Created on: Feb 15, 2026
 *      Author: kukun
 */

#include "dht11.h"

/* ==== CONFIG ==== */
#define DHT_PORT GPIOA
#define DHT_PIN  GPIO_PIN_4

/* ==== DWT Delay ==== */

static void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

/* ==== GPIO Helpers ==== */

static void set_output(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static void set_input(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT_PORT, &GPIO_InitStruct);
}

static dht11_status_t wait_for_state(GPIO_PinState state, uint32_t timeout_us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = timeout_us * (SystemCoreClock / 1000000);

    while (HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN) != state)
    {
        if ((DWT->CYCCNT - start) > ticks)
            return DHT11_TIMEOUT;
    }
    return DHT11_OK;
}

/* ==== Public ==== */

void dht11_init(void)
{
    dwt_init();
    set_output();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
}

dht11_status_t dht11_read(dht11_data_t *data)
{
    if (data == NULL)
        return DHT11_ERROR;

    uint8_t buffer[5] = {0};

    /* Start signal */
    set_output();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
    delay_us(20000);      // >18ms
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
    delay_us(30);
    set_input();

    /* Sensor response */
    if (wait_for_state(GPIO_PIN_RESET, 100) != DHT11_OK)
        return DHT11_TIMEOUT;

    if (wait_for_state(GPIO_PIN_SET, 100) != DHT11_OK)
        return DHT11_TIMEOUT;

    if (wait_for_state(GPIO_PIN_RESET, 100) != DHT11_OK)
        return DHT11_TIMEOUT;

    /* Read 40 bits */
    for (int i = 0; i < 40; i++)
    {
        if (wait_for_state(GPIO_PIN_SET, 70) != DHT11_OK)
            return DHT11_TIMEOUT;

        uint32_t start = DWT->CYCCNT;

        if (wait_for_state(GPIO_PIN_RESET, 100) != DHT11_OK)
            return DHT11_TIMEOUT;

        uint32_t pulse_width = DWT->CYCCNT - start;

        buffer[i / 8] <<= 1;

        if (pulse_width > (SystemCoreClock / 1000000) * 40)
            buffer[i / 8] |= 1;
    }

    /* Checksum */
    if ((uint8_t)(buffer[0] + buffer[1] + buffer[2] + buffer[3]) != buffer[4])
        return DHT11_CHECKSUM_ERROR;

    data->humidity = buffer[0];
    data->temperature = buffer[2];

    return DHT11_OK;
}

