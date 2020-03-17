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

extern void ConfigurePrms(void);
volatile static FRESULT fResult;

extern uint8_t g_dataStartPattern[512];

int main(void)
{
	for(register uint32_t i = 0; i < 1000000; i++);
	for(uint32_t i = 0; i < 512; i=i+4)
	{
		g_dataStartPattern[i] = 0xFF;
		g_dataStartPattern[i+1] = 0x00;
		g_dataStartPattern[i+2] = 0xAA;
		g_dataStartPattern[i+3] = 0xBB;
	}

	ConfigurePrms();
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









