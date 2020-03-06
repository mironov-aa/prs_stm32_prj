/*
 * PRMS_Tasks.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include "stm32f0xx.h"
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "PRMS_Tasks.h"

extern TaskHandle_t g_buttonHandler;
extern TaskHandle_t g_fpgaHandler;
extern TaskHandle_t g_memoryHandler;
extern uint8_t g_dataBuffer[512];
extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);

extern FIL g_file;


//Debug definition
static TaskStatus_t debugArray[5] = {0};
static uint32_t  totalRunTime = 0;//total run time since the target booted
static FRESULT fResult;

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
				vTaskSuspend(g_memoryHandler);
				GPIOC->BSRR |= GPIO_BSRR_BR_8;
				GPIOC->BSRR |= GPIO_BSRR_BR_9;
				fResult = f_close(&g_file);
			}
			else
			{
				vTaskResume(g_fpgaHandler);
				vTaskResume(g_memoryHandler);
				fResult = f_open(&g_file, "test.bin", FA_OPEN_APPEND | FA_WRITE);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
			xTaskGetTickCount();
		}
	}
}


void vFpgaTask(void* argument)
{
	g_fpgaHandler = xTaskGetCurrentTaskHandle();
	vTaskSuspend(g_fpgaHandler);
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS_9;
		vTaskDelay(1000);
		GPIOC->BSRR |= GPIO_BSRR_BR_9;
		vTaskDelay(1000);
	}
}

void vMemoryTask(void* argument)
{
	g_memoryHandler = xTaskGetCurrentTaskHandle();
	unsigned int savedBytes = 0;
	vTaskSuspend(g_memoryHandler);
	int i = 0;
	while(1)
	{
		TickType_t t1 = xTaskGetTickCount();
		fResult = f_write(&g_file, g_dataBuffer, 512, &savedBytes);
		TickType_t t2 = xTaskGetTickCount();
		t2 -= t1;
		if(t2 > 8)
		{
			i++;
		}
		vTaskDelay(320);
	}
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName)
{
	while(1);
}

void vApplicationIdleHook()
{
#ifdef FREERTOS_DEBUG
	UBaseType_t arraySize = 0;
	arraySize = uxTaskGetSystemState( debugArray, 5, &totalRunTime );
	configASSERT(arraySize);
#endif
}

