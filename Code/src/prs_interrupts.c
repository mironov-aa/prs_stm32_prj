/*
 * PRMS_Interrupts.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "prs_interrupts.h"
#include "usb_dcd_int.h"

extern TaskHandle_t xButtonHandler;

#ifdef FREERTOS_DEBUG
extern volatile uint32_t g_highFrequencyTimerTicks;
#endif

void EXTI0_1_IRQHandler(void)
{
    //Send notify to vButtonTask;
    xTaskNotifyFromISR(xButtonHandler,(uint32_t)~0, eSetBits, NULL);
    EXTI->PR |= EXTI_PR_PIF0;//Clear Interrupt
}

void TIM3_IRQHandler(void)
{
#ifdef FREERTOS_DEBUG
    if (TIM3->SR & TIM_SR_UIF)
    {
        TIM3->SR &= ~TIM_SR_UIF;
        g_highFrequencyTimerTicks++;
    }
#endif
}

void USB_IRQHandler(void)
{
    USB_Istr();
}
