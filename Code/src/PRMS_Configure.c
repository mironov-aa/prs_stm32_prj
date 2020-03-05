/*
 * GPIO_Configure.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include <stdbool.h>
#include "stm32f0xx.h"
#include "main.h"
#include "PRMS_Configure.h"



static inline void ConfigureGpio(void);
static inline void CongigureInterrupts(void); // Interrupts priority, NVIC & EXTI
static inline void ConfigureSpi1(bool isInitial);
static inline void ConfigureSpi2(void);
static inline void ConfigureTim3(void);

extern uint8_t g_isHseStart;
extern uint8_t SdhcCardInitialize();


void ConfigurePrms(void)
{
	ConfigureGpio();

	if(!g_isHseStart)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS_7; //Warning if HSE don't start!
	}

	CongigureInterrupts();
	ConfigureSpi1(true);
	SdhcCardInitialize();
	ConfigureSpi1(false);

	ConfigureSpi2();

#ifdef FREERTOS_DEBUG
	ConfigureTim3();
#endif
}

static inline void ConfigureGpio(void)
{
	//Enable clock for port A, B & C
	RCC->AHBENR |= (uint32_t)(RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);

//******************************  GPIOA  ******************************//

	//Configure SPI1 ping on GPIO: PA5(SCK)
	GPIOA->MODER &= ~GPIO_MODER_MODER5;
	GPIOA->MODER |= GPIO_MODER_MODER5_1; //Alternate function

	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5; //AF0

	//PA4 SPI 1 software chip select
	GPIOA->MODER &= ~GPIO_MODER_MODER4;
	GPIOA->MODER |= GPIO_MODER_MODER4_0; //General Purpose output mode
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4; //High speed
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4 ; //Push-pull

	//PA0 Configuration. User button
	GPIOA->MODER &= (uint32_t)(~GPIO_MODER_MODER0);	//Input Mode
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;//Pull down

//******************************  GPIOB  ******************************//

	//Configure SPI2 pins on GPIO: PB12(NSS) PB13(SCK) PB14(MISO) PB15(MOSI)

	GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 |
					  GPIO_MODER_MODER14 | GPIO_MODER_MODER15);

	GPIOB->MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 |
					 GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1); //Alternate function

	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL12 | GPIO_AFRH_AFSEL13 |
					   GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15); //AF0

	//Configure SPI1 pins on GPIO: PB4(MISO) PB5(MOSI)

	GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
	GPIOB->MODER |= (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1); //Alternate function

	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5); //AF0

//******************************  GPIOC  ******************************//

/*LEDs: PC6: Led Indicator
 * 		PC7: Warning(lit) or fatal error(blink) LED
 * 		PC8: vLed1Task indicator
 * 		PC9: vLed2Task indicator
*/
	GPIOC->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7 |
					  GPIO_MODER_MODER8 | GPIO_MODER_MODER9);

	GPIOC->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 |
					 GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0); //General Purpose output mode

	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7 |
						GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9); //Low speed

	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7 |
					   GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9); //Push-pull
}

static inline void CongigureInterrupts(void)
{
	//Configure NVIC for push button on PA0
	NVIC_EnableIRQ(EXTI0_1_IRQn);
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	//PA0 as source input
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;

	//Set up EXTI
	EXTI->RTSR |= EXTI_RTSR_RT0; //Rising Edge for line 0
	EXTI->IMR |= EXTI_IMR_IM0; //Interrupt mask for line 0
}

static inline void ConfigureSpi1(bool isInitial)
{
	/* Enable the peripheral clock SPI1 */
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/* Correct disable SPI1*/
	while((SPI1->SR & SPI_SR_FTLVL) != 0){} //no more data to transmit
	while((SPI1->SR & SPI_SR_BSY) != 0){} //the last data frame is processed
	SPI1->CR1 = 0; //disable SPI
	while((SPI1->SR & SPI_SR_FRLVL) != 0)
	{
		uint16_t data = SPI1->DR;	//read all the received data
	}

	/* Configure SPI1 in master */
	/* (1) Master selection, CPOL and CPHA at 1 , software slave management & Internal slave select */
	/* (2) BR: if need SDHC initialize: Fpclk/256, otherwise Fpclk/4
	/* (3) Slave select output enabled, RXNE IT, 8-bit Rx fifo */
	/* (4) Enable SPI1 */
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_CPHA | SPI_CR1_CPOL; /* (1) */
	SPI1->CR1 &= ~SPI_CR1_BR; /* (2) */
	SPI1->CR1 |= (isInitial)? (SPI_CR1_BR) : (SPI_CR1_BR_0);
	SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_SSOE | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; /* (3) */
	SPI1->CR1 |= SPI_CR1_SPE; /* (4) */
}

static inline void ConfigureSpi2(void)
{
	 /* Enable the peripheral clock SPI2 */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	/* Configure SPI2 in slave */
	/* nSS hard, slave, CPOL and CPHA at 1 (rising first edge) */
	/* (1) 8-bit Rx fifo */
	/* (2) Enable SPI2 */
	SPI2->CR1 = SPI_CR1_CPHA | SPI_CR1_CPOL;
	SPI2->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH; /* (1) */
	SPI2->CR1 |= SPI_CR1_SPE; /* (2) */

}

#ifdef FREERTOS_DEBUG
static inline void ConfigureTim3(void)
{
	//Configure TIM3 for configGENERATE_RUN_TIME_STATS:FreeRTOSConfig.h
	/* Enable the peripheral clock TIM3 */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 |= TIM_CR1_URS;//only counter overflow generate update interrupt
	TIM3->ARR = 480;// Update interrupt each 1/100kHZ.
	TIM3->DIER |= TIM_DIER_UIE;//Update interrupt enable
	TIM3->CR1 |= TIM_CR1_CEN;//Enable counter

	/* Configure IT */
	NVIC_SetPriority(TIM3_IRQn, 0);
	NVIC_EnableIRQ(TIM3_IRQn);
}
#endif





