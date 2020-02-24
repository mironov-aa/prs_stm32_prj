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
extern uint16_t g_dataArray[8];

void EXTI0_1_IRQHandler(void)
{
	xTaskNotifyFromISR(g_buttonHandler,(uint32_t)~0, eSetBits,NULL);//Send notify to vButtonTask;
	EXTI->PR |= EXTI_PR_PIF0;//Clear Interrupt
}
