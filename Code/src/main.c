#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "PRMS_Initialize.h"


extern void vLed1Task(void* argument);
extern void vLed2Task(void* argument);
extern void vButtonTask(void* argument);


int main(void)
{
	PrmsInitialize();

	xTaskCreate(vButtonTask,"BUTTON",128,NULL, 1, NULL);
	xTaskCreate(vLed1Task, "LED", 128, NULL, 1, NULL);
	xTaskCreate(vLed2Task, "LED1", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
}









