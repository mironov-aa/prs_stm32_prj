/*
 * GPIO_Initialize.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include "PRMS_Initialize.h"

void PrmsInitialize(void)
{
	GpioInitialize();

	if(!__IS_HSE_START__)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS_7; //Warning if HSE don't start!
	}

	InterruptInitialize();

}

void InterruptInitialize(void)
{
	//Configure NVIC for user button on PA0
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	//Set up EXTI
	EXTI->RTSR |= EXTI_RTSR_RT0; //Rising Edge for line 0
	EXTI->IMR |= EXTI_IMR_IM0; //Interrupt mask for line 0
}

void GpioInitialize(void)
{
	//Enable clock for port A and C
	RCC->AHBENR |= (uint32_t)(RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN);
//******************************  GPIOC  ******************************//

	//PC8 configuration. vLed1Task indicator
	GPIOC->MODER |= GPIO_MODER_MODER8_0;//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_8;	//push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_0;//medium speed

	//PC9 configuration. vLed2Task indicator
	GPIOC->MODER |= GPIO_MODER_MODER9_0;//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_9;	//push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0;//medium speed

	//PC7 configuration. Warning(lit) or fatal error(blink) LED
	GPIOC->MODER |= GPIO_MODER_MODER7_0;//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_7;//Push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_0;//medium speed

	//PC6 configuration. Led Indicator
	GPIOC->MODER |= GPIO_MODER_MODER6_0;	//General Purpose output mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_6;		//Push-pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0; //medium speed
//******************************  GPIOA  ******************************//

	//PA0 Configuration. User button
	GPIOA->MODER &= (uint32_t)(~GPIO_MODER_MODER0);	//Input Mode
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;//Pull down
}
