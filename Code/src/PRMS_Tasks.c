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

extern TaskHandle_t g_buttonHandler;
extern TaskHandle_t g_fpgaHandler;
extern TaskHandle_t g_ledHandler;

void vButtonTask(void* argument)
{
	g_buttonHandler = xTaskGetCurrentTaskHandle();
	uint32_t interruptFlag;
	while(1)
	{
		interruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		if(interruptFlag == ((uint32_t)~0))
		{
			if((GPIOC->ODR & GPIO_ODR_6))
			{
				vTaskSuspend(g_fpgaHandler);
				vTaskSuspend(g_ledHandler);
				GPIOC->BSRR |= GPIO_BSRR_BR_8;
				GPIOC->BSRR |= GPIO_BSRR_BR_9;
			}
			else
			{
				vTaskResume(g_fpgaHandler);
				vTaskResume(g_ledHandler);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
		}

	}
}

void vFpgaTask(void* argument)
{
	g_fpgaHandler = xTaskGetCurrentTaskHandle();
	vTaskSuspend(g_fpgaHandler);
	while(1)
	{
		//TODO: Chip select = 0;
		//TODO: 16byte!!!;
		for(uint16_t i = 0; i < (uint16_t)8; i++)//Start transmission(16byte). In the interrupt handler place received data in the g_dataBuffer[8];
		{
			while(!(SPI1->SR & SPI_SR_TXE)){};//Wait till transmit buffer be empty
			*(uint16_t*)&SPI2->DR = (uint16_t)i+1;//TODO: test bytes!!!
			*(uint16_t*)&SPI1->DR = 0;//Send dummy bytes that clock slave.
		}
		//TODO: Chip select = 1;
		//TODO: Notify to MemoryTask!
		vTaskDelay(10*portTICK_PERIOD_MS);
	}
}

void vLedTask(void* argument)
{
	g_ledHandler = xTaskGetCurrentTaskHandle();
	vTaskSuspend(g_ledHandler);
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

