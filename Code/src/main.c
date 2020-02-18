#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void GPIO_init();
void vLed1Task(void* argument);
void vLed2Task(void* argument);



int main(void)
{
	GPIO_init();

	xTaskCreate(vLed1Task, "LED", 32, NULL, 1, NULL);
	xTaskCreate(vLed2Task, "LED", 32, NULL, 1, NULL);
	vTaskStartScheduler();

	while(1)
	{

	}
}

void vLed1Task(void* argument)
{
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BR_8;
		vTaskDelay(1000);
		GPIOC->BSRR |= GPIO_BSRR_BS_8;
		vTaskDelay(1000);
	}
}

void vLed2Task(void* argument)
{
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS_9;
		vTaskDelay(1000);
		GPIOC->BSRR |= GPIO_BSRR_BR_9;
		vTaskDelay(1000);
	}
}


void GPIO_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	//PC8 configuration
	GPIOC->MODER |= GPIO_MODER_MODER8_0;	//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_8;		//push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_0; // medium speed
	//PC9 configuration
	GPIOC->MODER |= GPIO_MODER_MODER9_0;	//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_9;		//push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0; // medium speed
}



