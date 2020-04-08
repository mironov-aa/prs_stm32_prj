#include "stm32f072xb.h"
#include "global.h"
#include "PRMS_Configure.h"
#include "PRMS_Tasks.h"
#include "main.h"

int main(void)
{
	for(uint32_t i = 0; i < 512; i=i+4)
	{
		g_dataStartPattern[i] = 0xFF;
		g_dataStartPattern[i+1] = 0x00;
		g_dataStartPattern[i+2] = 0xAA;
		g_dataStartPattern[i+3] = 0xBB;
	}

	ConfigurePrms();
	ConfigureFreeRtosTasks();

	vTaskStartScheduler();
	for(;;);
}









