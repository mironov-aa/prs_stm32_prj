#include "FreeRTOS.h"
#include "task.h"

extern void ConfigurePrms(void);
extern void vButtonTask(void* argument);
extern void vFpgaTask(void* argument);
extern void vLedTask(void* argument);

int main(void)
{
	ConfigurePrms();

	xTaskCreate(vButtonTask,"BUTTON",128,NULL, 1, NULL);
	xTaskCreate(vFpgaTask, "LED", 128, NULL, 1, NULL);
	xTaskCreate(vLedTask, "LED1", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
}









