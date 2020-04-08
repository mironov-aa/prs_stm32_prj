#include "PRMS_Configure.h"
#include "PRMS_Tasks.h"
#include "main.h"

int main(void)
{

	ConfigurePrms();
	ConfigureFreeRtosTasks();

	vTaskStartScheduler();
	for(;;);
}









