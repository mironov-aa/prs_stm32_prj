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
	*(uint8_t*)&SPI1->DR = dataIn;

	while(!(SPI1->SR & SPI_SR_RXNE)) {}  // wait for byte to be received
	uint8_t dataOut = *(uint8_t*)&SPI1->DR;

	return dataOut;
}

