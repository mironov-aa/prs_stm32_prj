/*
 * PRMS_Tasks.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PRMS_Tasks.h"

void vButtonTask(void* argument)
{
	__BUTTON_HANDLER__ = xTaskGetCurrentTaskHandle();
	uint32_t InterruptFlag;
	while(1)
	{
		InterruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		if(InterruptFlag == ((uint32_t)~0))
		{
			if((GPIOC->ODR & GPIO_ODR_6))
			{
				vTaskSuspend(__LED1_HANDLER__);
				vTaskSuspend(__LED2_HANDLER__);
				GPIOC->BSRR |= GPIO_BSRR_BR_8;
				GPIOC->BSRR |= GPIO_BSRR_BR_9;
			}
			else
			{
				vTaskResume(__LED1_HANDLER__);
				vTaskResume(__LED2_HANDLER__);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
		}

	}
}

void vLed1Task(void* argument)
{
	__LED1_HANDLER__ = xTaskGetCurrentTaskHandle();
	vTaskSuspend(__LED1_HANDLER__);
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BR_8;
		vTaskDelay(1000);
		GPIOC->BSRR |= GPIO_BSRR_BS_8;
		vTaskDelay(1000);
	}
}

void vLed2Task(void* argument)
{
	__LED2_HANDLER__ = xTaskGetCurrentTaskHandle();
	vTaskSuspend(__LED2_HANDLER__);
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS_9;
		vTaskDelay(1000);
		GPIOC->BSRR |= GPIO_BSRR_BR_9;
		vTaskDelay(1000);
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName)
{
	while(1);
}

