#include "stm32f072xb.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

extern void ConfigurePrms(void);
extern void vButtonTask(void* argument);
extern void vFpgaTask(void* argument);
extern void vMemoryTask(void* argument);

extern uint8_t SdhcCardInitialize();
extern uint8_t g_dataBuffer[512];
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);

int main(void)
{
	ConfigurePrms();
	xTaskCreate(vButtonTask,"BUTTON",128,NULL, 1, NULL);
	xTaskCreate(vFpgaTask, "FPGA", 128, NULL, 1, NULL);
	xTaskCreate(vMemoryTask, "MEM", 128, NULL, 1, NULL);
	vTaskStartScheduler();
	for(;;);
}









