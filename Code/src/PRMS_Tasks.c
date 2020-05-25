/*
 * PRMS_Tasks.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */

#include "main.h"
#include "global.h"
#include  "usbd_msc_core.h"
#include  "usbd_usr.h"
#include "spi_driver.h"
#include "SDHC_card_driver.h"
#include "PRMS_Tasks.h"

/*________________________Data buffers____________________________*/
static uint8_t dataBuffer[1024];



static uint8_t dataStartPattern[512] = {0};

/*________________________Debug__________________________________*/
#ifdef FREERTOS_DEBUG
static TaskStatus_t debugArray[5] = {0};
static uint32_t  totalRunTime = 0;//total run time since the target booted
#endif

/*________________________FatFS__________________________________*/
static FRESULT fResult;
static FATFS g_fatFs;
static FIL g_file;

/*______________________FreeRTOS___________________________*/
//Tasks buffer
static StaticTask_t xButtonBuffer;
static StaticTask_t xMemoryBuffer;
static StaticTask_t xFpgaBuffer;

//Tasks stack
static StackType_t xButtonStack[128];
static StackType_t xMemoryStack[128];
static StackType_t xFpgaStack[128];

//Tasks handler
TaskHandle_t xButtonHandler = NULL;
static TaskHandle_t xMemoryHandler = NULL;
static TaskHandle_t xFpgaHandler = NULL;

//Tasks function
static void vButtonTask(void* argument);
static void vMemoryTask(void* argument);
static void vFpgaTask(void* argument);

/*_____________________________USB Library______________________*/
USB_CORE_HANDLE  USB_Device_dev;


void ConfigureFreeRtosTasks(void)
{
	for(uint32_t i = 0; i < 512; i=i+4)//TODO: delete me!!!
	{
		dataStartPattern[i] = 0xFF;
		dataStartPattern[i+1] = 0x00;
		dataStartPattern[i+2] = 0xAA;
		dataStartPattern[i+3] = 0xBB;
	}


	xButtonHandler = xTaskCreateStatic(vButtonTask, "BUTTON", sizeof(xButtonStack)/4, NULL, 4, xButtonStack, &xButtonBuffer);
	xMemoryHandler = xTaskCreateStatic(vMemoryTask, "MEM", sizeof(xMemoryStack)/4, NULL, 2, xMemoryStack, &xMemoryBuffer);
	xFpgaHandler = xTaskCreateStatic(vFpgaTask, "FPGA", sizeof(xFpgaStack)/4, NULL, 3, xFpgaStack, &xFpgaBuffer);
}

static void vButtonTask(void* argument)
{
	volatile uint32_t interruptFlag;
	unsigned int savedBytes = 0;
	USBD_Init(&USB_Device_dev,&USR_desc, &USBD_MSC_cb, &USR_cb);
	while(1)
	{
		interruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		portENTER_CRITICAL();
		if(interruptFlag == ((uint32_t)~0))
		{
			if((GPIOC->ODR & GPIO_ODR_6))
			{
				vTaskSuspend(xFpgaHandler);
				vTaskSuspend(xMemoryHandler);
				fResult = f_close(&g_file);
				fResult = f_unmount("");
				DCD_DevConnect(&USB_Device_dev);
			}
			else
			{
				DCD_DevDisconnect(&USB_Device_dev);
				vTaskResume(xFpgaHandler);
				vTaskResume(xMemoryHandler);
				fResult = f_mount(&g_fatFs, "", 1);
				fResult = f_open(&g_file, "test.bin", FA_OPEN_APPEND | FA_WRITE);
				fResult = f_write(&g_file, dataStartPattern, 512, &savedBytes);
			}
			GPIOC->ODR ^= GPIO_ODR_6;
		}
		portEXIT_CRITICAL();
	}
}


static void vFpgaTask(void* argument)
{
	uint8_t *const firstBlockPointer = &dataBuffer[0];
	uint8_t *const secondBlockPointer = &dataBuffer[512];
	uint8_t *const endPointer = &dataBuffer[1024];
	uint8_t* currentPointer = firstBlockPointer;
	vTaskSuspend(xFpgaHandler);
	while(1)
	{
		while(currentPointer < secondBlockPointer)
		{
			SPI2_OnlyReceive((uint16_t*)currentPointer, 8);
			currentPointer += 16;
			vTaskDelay(2*portTICK_PERIOD_MS);
		}
		xTaskNotify(xMemoryHandler,(uint32_t)firstBlockPointer, eSetBits);

		while(currentPointer < endPointer)
		{
			SPI2_OnlyReceive((uint16_t*)currentPointer, 8);
			currentPointer += 16;
			vTaskDelay(2*portTICK_PERIOD_MS);
		}
		xTaskNotify(xMemoryHandler,(uint32_t)secondBlockPointer, eSetBits);
		currentPointer = firstBlockPointer;
	}
}


static void vMemoryTask(void* argument)
{
	unsigned int savedBytes = 0;
	uint32_t dataAddress = 0;
	vTaskSuspend(xMemoryHandler);
	while(1)
	{
		dataAddress = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
		if(dataAddress != 0)
		{
			fResult = f_write(&g_file, (uint8_t*)dataAddress, 512, &savedBytes);
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
};


