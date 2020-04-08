/*
 * spi_driver.c
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */


#include "spi_driver.h"
#include "global.h"

uint8_t SPI1_TransmitReceive(uint8_t dataIn)
{
	while(!(SPI1->SR & SPI_SR_TXE)) {}  //Wait till transmit buffer be empty
	__MEMORY_BARRIER;
	*(uint8_t*)&SPI1->DR = dataIn;

	while(!(SPI1->SR & SPI_SR_RXNE)) {}  // wait for byte to be received
	__MEMORY_BARRIER;
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

	__MEMORY_BARRIER;
	DMA1_Channel3->CCR |= DMA_CCR_EN;
	DMA1_Channel2->CCR = DMA_CCR_EN; //without Memory increment, read from peripheral, channel enable

	__MEMORY_BARRIER;
	while(!(DMA1->ISR & DMA_ISR_TCIF3)){}//wait till all transmit
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF3;//clear flag
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;//disable DMA

	while(!(DMA1->ISR & DMA_ISR_TCIF2)){}//wait till all receive
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF2;//clear flag
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;//disable DMA
}

void SPI1_OnlyReceiveBlockDMA(uint8_t* bufferOut)
{
	uint8_t dummy_send = 0xFF;

	/* DMA1 Channel2 SPI1_RX config */
	DMA1_Channel2->CCR = DMA_CCR_MINC;//memory increment, read from peripheral
	DMA1_Channel2->CMAR = (uint32_t)bufferOut;
	DMA1_Channel2->CNDTR = 512; //Data size
	/* DMA1 Channel3 SPI1_TX config */
	DMA1_Channel3->CCR = DMA_CCR_DIR; //read from memory
	DMA1_Channel3->CMAR = (uint32_t)&dummy_send;
	DMA1_Channel3->CNDTR = 512; //Data size

	__MEMORY_BARRIER;
	DMA1_Channel3->CCR |= DMA_CCR_EN;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	__MEMORY_BARRIER;
	while(!(DMA1->ISR & DMA_ISR_TCIF3)){}//wait till all transmit
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF3;//clear flag
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;//disable DMA

	while(!(DMA1->ISR & DMA_ISR_TCIF2)){}//wait till all receive
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF2;//clear flag
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;//disable DMA
}

void SPI2_OnlyReceive(uint16_t* bufferOut, uint32_t countHalfWords)
{
	uint8_t dummy_send = 0xFF;

	/* DMA1 Channel4 SPI2_RX config */
	DMA1_Channel4->CCR = DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;//memory increment, read from peripheral, 16bit
	DMA1_Channel4->CMAR = (uint32_t)bufferOut;
	DMA1_Channel4->CNDTR = countHalfWords; //Data size
	/* DMA1 Channel5 SPI2_TX config */
	DMA1_Channel5->CCR = DMA_CCR_DIR | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0; //read from memory
	DMA1_Channel5->CMAR = (uint32_t)&dummy_send;
	DMA1_Channel5->CNDTR = countHalfWords; //Data size

	__MEMORY_BARRIER;
	DMA1_Channel4->CCR |= DMA_CCR_EN;
	DMA1_Channel5->CCR |= DMA_CCR_EN;

	__MEMORY_BARRIER;
	while(!(DMA1->ISR & DMA_ISR_TCIF5)){}//wait till all transmit
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF5;//clear flag
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;//disable DMA

	while(!(DMA1->ISR & DMA_ISR_TCIF4)){}//wait till all receive
	__MEMORY_BARRIER;
	DMA1->IFCR |= DMA_IFCR_CTCIF4;//clear flag
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;//disable DMA
}

