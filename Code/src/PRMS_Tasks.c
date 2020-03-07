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

extern TaskHandle_t xButtonHandler;
extern TaskHandle_t xMemoryHandler;
extern TaskHandle_t xFpgaHandler;

extern uint8_t g_dataBuffer[512];
extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
extern void g_Delay(uint32_t timeout);
extern FIL g_file;


//Debug definition
static TaskStatus_t debugArray[5] = {0};
static uint32_t  totalRunTime = 0;//total run time since the target booted
static FRESULT fResult;

void vButtonTask(void* argument)
{
	uint32_t interruptFlag;
	while(1)
	{
		interruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		if(interruptFlag == ((uint32_t)~0))
		{
			if((GPIOC->ODR & GPIO_ODR_6))
			{
				vTaskSuspend(xMemoryHandler);
				vTaskSuspend(xFpgaHandler);
				GPIOC->BSRR |= GPIO_BSRR_BR_8;
				GPIOC->BSRR |= GPIO_BSRR_BR_9;
				fResult = f_close(&g_file);
			}
			else
			{
				vTaskResume(xMemoryHandler);
				vTaskResume(xFpgaHandler);
				fResult = f_open(&g_file, "test.bin", FA_OPEN_APPEND | FA_WRITE);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
		}
	}
}


void vFpgaTask(void* argument)
{
	uint32_t i = 0;
	vTaskSuspend(xFpgaHandler);
	while(1)
	{
		g_Delay(40000);
		i++;
		if(i == 32)
		{
			i = 0;
			xTaskNotify(xMemoryHandler,~(0), eSetBits);
		}
		vTaskDelay(10*portTICK_PERIOD_MS);
	}
}

void vMemoryTask(void* argument)
{
	unsigned int savedBytes = 0;
	uint32_t errorsCounter = 0;
	uint64_t counter = 0;
	uint32_t flag = 0;
	uint32_t maxValue = 0;
	vTaskSuspend(xMemoryHandler);
	while(1)
	{
		flag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		TickType_t t1 = xTaskGetTickCount();
		fResult = f_write(&g_file, g_dataBuffer, 512, &savedBytes);
		TickType_t t2 = xTaskGetTickCount();
		t2-=t1;
		counter++;
		if(t2 > 9)
		{
			maxValue = (t2 > maxValue)? t2 : maxValue;
			errorsCounter++;
		}

	}
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize)
{

	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
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

