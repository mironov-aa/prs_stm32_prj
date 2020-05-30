#include "prs_configure.h"
#include "prs_tasks.h"
#include "main.h"

int main(void)
{
	ConfigurePrs();
	ConfigureFreeRtosTasks();
	vTaskStartScheduler();
	for(;;);
}









