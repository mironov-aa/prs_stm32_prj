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

extern uint8_t g_dataBuffer1[512];
extern uint8_t g_dataStartPattern[512];

extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
extern void g_Delay(uint32_t timeout);

extern FIL g_file;
extern FATFS g_fatFs;

#include  "usbd_msc_core.h"
#include  "usbd_usr.h"
USB_CORE_HANDLE  USB_Device_dev;


//Debug definition
static TaskStatus_t debugArray[5] = {0};
static uint32_t  totalRunTime = 0;//total run time since the target booted
volatile static FRESULT fResult;

void vButtonTask(void* argument)
{
	volatile uint32_t interruptFlag;
	unsigned int savedBytes = 0;
	USBD_Init(&USB_Device_dev,&USR_desc, &USBD_MSC_cb, &USR_cb);
	while(1)
	{
		interruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		__asm volatile( "dmb" ::: "memory" );
		portENTER_CRITICAL();
		__asm volatile( "dmb" ::: "memory" );
		if(interruptFlag == ((uint32_t)~0))
		{
			if((GPIOC->ODR & GPIO_ODR_6))
			{
				vTaskSuspend(xMemoryHandler);
				vTaskSuspend(xFpgaHandler);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_sync(&g_file);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_close(&g_file);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_unmount("");
				__asm volatile( "dmb" ::: "memory" );
				DCD_DevConnect(&USB_Device_dev);
			}
			else
			{
				DCD_DevDisconnect(&USB_Device_dev);
				__asm volatile( "dmb" ::: "memory" );
				vTaskResume(xMemoryHandler);
				vTaskResume(xFpgaHandler);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_mount(&g_fatFs, "", 1);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_open(&g_file, "test.bin", FA_OPEN_APPEND | FA_WRITE);
				__asm volatile( "dmb" ::: "memory" );
				fResult = f_write(&g_file, g_dataStartPattern, 512, &savedBytes);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
		}
		__asm volatile( "dmb" ::: "memory" );
		portEXIT_CRITICAL();
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
		vTaskDelay(2*portTICK_PERIOD_MS);
	}
}

void vMemoryTask(void* argument)
{
	unsigned int savedBytes = 0;
	volatile uint32_t errorsCounter = 0;
	volatile uint64_t counter = 0;
	uint32_t flag = 0;
	uint32_t maxValue = 0;
	vTaskSuspend(xMemoryHandler);
	while(1)
	{
		flag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		volatile TickType_t t1 = xTaskGetTickCount();
		__asm volatile( "dmb" ::: "memory" );
		fResult = f_write(&g_file, g_dataBuffer1, 512, &savedBytes);
		__asm volatile( "dmb" ::: "memory" );
		volatile TickType_t t2 = xTaskGetTickCount();
		t2-=t1;
		counter++;
		if(t2 > 2)
		{
			maxValue = (t2  > maxValue)? t2 : maxValue;
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

