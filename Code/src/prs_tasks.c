/*
 * prs_tasks.c
 *
 *  Created on: Feb 21, 2020
 *  Author: mironov-aa
 */

#include "main.h"
#include "stm32f072xb.h"
#include "stdio.h"
#include "stdlib.h"
#include "global.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "spi_driver.h"
#include "sdhc_card_driver.h"
#include "prs_tasks.h"


/*________________________Data buffers____________________________*/
static uint8_t dataBuffer[1024];

/*________________________Debug__________________________________*/
#ifdef FREERTOS_DEBUG
static TaskStatus_t debugArray[5] = {0};
static uint32_t  totalRunTime = 0;//total run time since the target booted
#endif

/*________________________FatFS__________________________________*/
static FRESULT fResult;
static FATFS fatFs;
static FIL file;

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
//Queue
static QueueHandle_t xQueueHandler;
static StaticQueue_t xQueue;
static uint8_t ucQueueStorageArea[4 * sizeof(uint32_t)];
//Mutex
SemaphoreHandle_t xMutex = NULL;
StaticSemaphore_t xMutexBuffer;

/*_____________________________USB Library______________________*/
USB_CORE_HANDLE  USB_Device_dev;


void ConfigureFreeRtosTasks(void)
{
    xQueueHandler  = xQueueCreateStatic(4,
                                        sizeof(uint32_t),
                                        ucQueueStorageArea,
                                        &xQueue);
    
    xButtonHandler = xTaskCreateStatic(vButtonTask, 
                                       "BUTTON", 
                                       sizeof(xButtonStack)/4, NULL, 
                                       4, 
                                       xButtonStack,
                                       &xButtonBuffer);
                                       
    xMemoryHandler = xTaskCreateStatic(vMemoryTask, 
                                       "MEM",
                                       sizeof(xMemoryStack)/4, 
                                       NULL, 
                                       2, 
                                       xMemoryStack, 
                                       &xMemoryBuffer);

    xFpgaHandler = xTaskCreateStatic(vFpgaTask, 
                                     "FPGA", 
                                     sizeof(xFpgaStack)/4, 
                                     NULL, 
                                     3, 
                                     xFpgaStack, 
                                     &xFpgaBuffer);
}

static void vButtonTask(void* argument)
{
    volatile uint32_t interruptFlag;
    uint32_t savedBytes = 0;
    //time: Hh:Mm:Ss
    uint8_t charBuffer[10] = {'H','h','M','m','S','s','.','b','i','n'};
    USBD_Init(&USB_Device_dev,&USR_desc, &USBD_MSC_cb, &USR_cb);
    NVIC_EnableIRQ(EXTI0_1_IRQn); //Enable push button EXTI
    while(1)
    {
        interruptFlag = ulTaskNotifyTake((uint32_t)~0, portMAX_DELAY);
        portENTER_CRITICAL();
        if(interruptFlag == ((uint32_t)~0))
        {
            if((GPIOC->ODR & GPIO_ODR_6))
            {
                vTaskSuspend(xFpgaHandler);
                if(xSemaphoreTake(xMutex, (TickType_t)5) == pdTRUE)
                {
                    f_close(&file);
                    f_unmount("");
                    DCD_DevConnect(&USB_Device_dev);
                    xSemaphoreGive(xMutex);
                }
            }
            else
            {
                itoa(RTC->TR, charBuffer, 16);//generate file name
                if( charBuffer[5] == '\n' )
                {
                    charBuffer[5] = '.';
                    charBuffer[6] = 'b';
                    charBuffer[7] = 'i';
                    charBuffer[8] = 'n';
                    charBuffer[9] = '\n';
                }
                else
                {
                    charBuffer[6] = '.';
                    charBuffer[7] = 'b';
                    charBuffer[8] = 'i';
                    charBuffer[9] = 'n';
                }
                DCD_DevDisconnect(&USB_Device_dev);
                vTaskResume(xFpgaHandler);
                if(xSemaphoreTake(xMutex, (TickType_t)5) == pdTRUE)
                {
                    f_mount(&fatFs, "", 1);
                    f_open(&file, charBuffer, FA_OPEN_APPEND | 
                                              FA_WRITE);
                    xSemaphoreGive(xMutex);
                }
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
            SPI2_OnlyReceive(currentPointer, 
                             g_settings.numberOfCounters);
            currentPointer += g_settings.numberOfCounters;
            vTaskDelay(g_settings.periodMs);
        }        
        xQueueSend(xQueueHandler, (void*)firstBlockPointer, (TickType_t)5);

        while(currentPointer < endPointer)
        {
            SPI2_OnlyReceive(currentPointer, 
                             g_settings.numberOfCounters);
            currentPointer += g_settings.numberOfCounters;
            vTaskDelay(g_settings.periodMs);
        }
        xQueueSend(xQueueHandler, (void*)secondBlockPointer, (TickType_t)5);
        currentPointer = firstBlockPointer;
    }
}


static void vMemoryTask(void* argument)
{
    unsigned int savedBytes = 0;
    uint32_t dataAddress = 0;
    while(1)
    {
        xQueueReceive(xQueueHandler, (void*)&dataAddress, portMAX_DELAY);
        if(xSemaphoreTake(xMutex, (TickType_t) 1 ) == pdTRUE)
        {
            f_write(&file, (uint8_t*)dataAddress, 512, &savedBytes);
            xSemaphoreGive(xMutex);
        }
    }
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
                                   StackType_t **ppxIdleTaskStackBuffer,
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


