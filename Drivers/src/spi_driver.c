/*
 * spi_driver.c
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */


#include "spi_driver.h"

uint8_t SPI1_TransmitReceive(uint8_t dataIn)
{
	while(!(SPI1->SR & SPI_SR_TXE)) {}  //Wait till transmit buffer be empty
	__asm volatile( "dmb" ::: "memory" );
	*(uint8_t*)&SPI1->DR = dataIn;

	while(!(SPI1->SR & SPI_SR_RXNE)) {}  // wait for byte to be received
	__asm volatile( "dmb" ::: "memory" );
	uint8_t dataOut = *(uint8_t*)&SPI1->DR;

	return dataOut;
}

void SPI1_OnlyTransmitBlockDMA(uint8_t* bufferIn)
{
	uint8_t dummy_receive = 0;

	/* DMA1 Channel2 SPI1_RX config */
	DMA1_Channel2->CCR = 0;//restore default
	DMA1_Channel2->CMAR = (uint32_t)&dummy_receive;
	DMA1_Channel2->CNDTR = 512; //Data size
	/* DMA1 Channel3 SPI1_TX config */
	DMA1_Channel3->CCR = DMA_CCR_DIR | DMA_CCR_MINC; //memory increment, read from peripheral
	DMA1_Channel3->CMAR = (uint32_t)bufferIn;
	DMA1_Channel3->CNDTR = 512; //Data size

	__asm volatile( "dmb" ::: "memory" );
	DMA1_Channel3->CCR |= DMA_CCR_EN;
	DMA1_Channel2->CCR = DMA_CCR_EN; //without Memory increment, read from peripheral, channel enable

	__asm volatile( "dmb" ::: "memory" );
	while(!(DMA1->ISR & DMA_ISR_TCIF3)){}//wait till all transmit
	__asm volatile( "dmb" ::: "memory" );
	DMA1->IFCR |= DMA_IFCR_CTCIF3;//clear flag
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;//disable DMA

	while(!(DMA1->ISR & DMA_ISR_TCIF2)){}//wait till all receive
	__asm volatile( "dmb" ::: "memory" );
	DMA1->IFCR |= DMA_IFCR_CTCIF2;//clear flag
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;//disable DMA
}
