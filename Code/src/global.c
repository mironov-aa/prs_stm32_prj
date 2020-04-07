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
#include "main.h"

uint8_t g_isHseStart = 0; //< Set if HSE don't start after system_stm32f0xx.c::SetSysClock();


uint8_t g_dataStartPattern[512];



#ifdef FREERTOS_DEBUG
volatile uint32_t g_highFrequencyTimerTicks = 0;
#endif



void g_Delay(uint32_t timeout)
{
	for(volatile uint32_t i = 0; i < timeout; i++){}
}

void g_ErrorHandler(uint8_t errorCode)
{

	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		for(int i = 0; i < 2*errorCode; i++)
		{
			g_Delay(900000);
			GPIOC->ODR ^= GPIO_ODR_7;
			g_Delay(900000);
		}
		g_Delay(48000000);
	}
}
