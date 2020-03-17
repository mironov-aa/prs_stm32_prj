#include "stm32f072xb.h"
#include "ff.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include  "usbd_msc_core.h"
#include  "usbd_usr.h"
USB_CORE_HANDLE  USB_Device_dev ;

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
extern FATFS g_fatFs;

volatile static FRESULT fResult;

int main(void)
{
	ConfigurePrms();

	  USBD_Init(&USB_Device_dev,
	            &USR_desc,
	            &USBD_MSC_cb,
	            &USR_cb);

	while(1);
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









