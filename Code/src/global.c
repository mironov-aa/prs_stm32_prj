/*
 * global.c
 *
 *  Created on: Feb 24, 2020
 *      Author: mironov-aa
 */
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

uint8_t g_isHseStart = 0; //< Set if HSE don't start after system_stm32f0xx.c::SetSysClock();
TaskHandle_t g_buttonHandler = NULL;
TaskHandle_t g_fpgaHandler = NULL;
TaskHandle_t g_memoryHandler = NULL;

uint8_t g_dataBuffer[512] = {0};
FATFS g_fatFs;
FIL g_file;

#ifdef FREERTOS_DEBUG
volatile uint32_t g_highFrequencyTimerTicks = 0;
#endif



void g_Delay(uint32_t timeout)
{
	for(uint32_t i = 0; i < timeout; i++){}
}

void g_ErrorHandler(uint8_t errorCode)
{

	taskDISABLE_INTERRUPTS();
	for( ;; );
}
