#include "stm32f072xb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

extern void ConfigurePrms(void);
extern void vButtonTask(void* argument);
extern void vFpgaTask(void* argument);
extern void vLedTask(void* argument);

int main(void)
{
	ConfigurePrms();
	xTaskCreate(vButtonTask,"BUTTON",128,NULL, 1, NULL);
	xTaskCreate(vFpgaTask, "FPGA", 128, NULL, 1, NULL);
	xTaskCreate(vLedTask, "LED", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
}









