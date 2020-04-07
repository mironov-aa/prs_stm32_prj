/*
 * PRMS_Tasks.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */

#include "main.h"
#include  "usbd_msc_core.h"
#include  "usbd_usr.h"
#include "PRMS_Tasks.h"


extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
extern void g_Delay(uint32_t timeout);
extern uint8_t g_dataStartPattern[512];

/*________________________Data buffers____________________________*/
static uint8_t g_dataBuffer1[512];
static uint8_t g_dataBuffer2[512];

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
static StackType_t xButtonStack[512];
static StackType_t xMemoryStack[512];
static StackType_t xFpgaStack[512];

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
	xButtonHandler = xTaskCreateStatic(vButtonTask, "BUTTON", 512, NULL, 4, xButtonStack, &xButtonBuffer);
	xMemoryHandler = xTaskCreateStatic(vMemoryTask, "MEM", 512, NULL, 2, xMemoryStack, &xMemoryBuffer);
	xFpgaHandler = xTaskCreateStatic(vFpgaTask, "FPGA", 512, NULL, 3, xFpgaStack, &xFpgaBuffer);
}

static void vButtonTask(void* argument)
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


static void vFpgaTask(void* argument)
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


static void vMemoryTask(void* argument)
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

