/*
 * global.c
 *
 *  Created on: Feb 24, 2020
 *      Author: mironov-aa
 */

#include "FreeRTOS.h"
#include "task.h"


uint8_t g_isHseStart = 0; //< Set if HSE don't start after system_stm32f0xx.c::SetSysClock();
TaskHandle_t g_buttonHandler = NULL;
TaskHandle_t g_fpgaHandler = NULL;
TaskHandle_t g_ledHandler = NULL;

volatile uint16_t g_dataArray[8] = {0};
