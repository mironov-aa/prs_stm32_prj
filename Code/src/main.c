#include "stm32f072xb.h"
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"



//PRMS tasks
StaticTask_t xButtonBuffer;
StackType_t xButtonStack[512];
TaskHandle_t xButtonHandler = NULL;
extern void vButtonTask(void* argument);

StaticTask_t xMemoryBuffer;
StackType_t xMemoryStack[512];
TaskHandle_t xMemoryHandler = NULL;
extern void vMemoryTask(void* argument);

StaticTask_t xFpgaBuffer;
StackType_t xFpgaStack[512];
TaskHandle_t xFpgaHandler = NULL;
extern void vFpgaTask(void* argument);

//Initialize
extern void ConfigurePrms(void);

extern uint8_t g_dataBuffer[512];
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);

extern FATFS g_fatFs;
static FRESULT fResult;

int main(void)
{
	ConfigurePrms();

	for(uint32_t i = 0; i < 512; i++)
	{
		g_dataBuffer[i] = 0xBA;
	}

	fResult = f_mount(&g_fatFs, "", 1);

	xButtonHandler = xTaskCreateStatic(vButtonTask, "BUTTON", 1024, NULL, 4, xButtonStack, &xButtonBuffer);
	xMemoryHandler = xTaskCreateStatic(vMemoryTask, "MEM", 1024, NULL, 2, xMemoryStack, &xMemoryBuffer);
	xFpgaHandler = xTaskCreateStatic(vFpgaTask, "FPGA", 1024, NULL, 3, xFpgaStack, &xFpgaBuffer);
/*
	xTaskCreate(vButtonTask,"BUTTON",1024,NULL, 1, NULL);
	xTaskCreate(vFpgaTask, "FPGA", 1024, NULL, 2, NULL);
	xTaskCreate(vMemoryTask, "MEM", 1024, NULL, 1, NULL);
	*/
	vTaskStartScheduler();
	for(;;);
}









