/*
 * global.c
 *
 *  Created on: Feb 24, 2020
 *      Author: mironov-aa
 */
#include "ff.h"
#include "FreeRTOS.h"
#include "stm32f072xb.h"
#include "task.h"
#include "global.h"

#ifdef FREERTOS_DEBUG
volatile uint32_t g_highFrequencyTimerTicks = 0;
#endif

void G_Delay(uint32_t timeout)
{
	for(volatile uint32_t i = 0; i < timeout; i++){}
}

void G_ErrorHandler(uint8_t errorCode)
{
	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		for(int i = 0; i < 2*errorCode; i++)
		{
			G_Delay(900000);
			GPIOC->ODR ^= GPIO_ODR_7;
			G_Delay(900000);
		}
		G_Delay(48000000);
	}
}

