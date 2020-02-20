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

extern TaskHandle_t __BUTTON_HANDLER__;

void EXTI0_1_IRQHandler(void)
{
	xTaskNotifyFromISR(__BUTTON_HANDLER__,(uint32_t)~0, eSetBits,NULL);//Send notify to vButtonTask;
	EXTI->PR |= EXTI_PR_PIF0;//Clear Interrupt
}
