/*
 * PRMS_Interrupts.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PRMS_Interrupts.h"

extern TaskHandle_t g_buttonHandler;
extern uint16_t g_dataBuffer[8];

void EXTI0_1_IRQHandler(void)
{
	xTaskNotifyFromISR(g_buttonHandler,(uint32_t)~0, eSetBits,NULL);//Send notify to vButtonTask;
	EXTI->PR |= EXTI_PR_PIF0;//Clear Interrupt
}

void SPI1_IRQHandler(void)
{
	static int i = 0;
	if((SPI1->SR & SPI_SR_RXNE) == SPI_SR_RXNE)
	{
		g_dataBuffer[i] = (uint16_t)SPI1->DR; /* receive data, clear flag */
		i = ((i < 7)? (i + 1) : 0);
	}
}
