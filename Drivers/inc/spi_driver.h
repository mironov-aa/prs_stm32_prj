/*
 * spi_driver.h
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */

#ifndef INC_SPI_DRIVER_H_
#define INC_SPI_DRIVER_H_

#include <stdint.h>
#include "stm32f072xb.h"

//SPI1: SD_CARD
// PA5(SCK)
// PA4(CS)
// PB4(MISO)
// PB5(MOSI)
uint8_t SPI1_TransmitReceive(uint8_t dataIn);
void SPI1_OnlyTransmitBlockDMA(uint8_t* bufferIn);

static inline void SPI1_CsLow()
{
	GPIOA->BSRR |= GPIO_BSRR_BR_4;
}

static inline void SPI1_CsHigh()
{
	GPIOA->BSRR |= GPIO_BSRR_BS_4;
}

//SPI2: FPGA
uint16_t SPI2_TransmitReceive(uint16_t dataIn);



#endif /* INC_SPI_DRIVER_H_ */
