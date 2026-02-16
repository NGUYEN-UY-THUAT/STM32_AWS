/**
 ******************************************************************************
 * @file    retarget.c
 * @brief   Retarget printf to UART (STM32CubeIDE safe)
 ******************************************************************************
 */

#include "stm32f4xx_hal.h"
#include <stdio.h>

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif


/* UART debug */
extern UART_HandleTypeDef huart5;

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
