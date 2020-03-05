/*
 * SDHC_card_driver.h
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */
#ifndef INC_SDHC_CARD_DRIVER_H_
#define INC_SDHC_CARD_DRIVER_H_

#include <stdint.h>

uint8_t SdhcCardInitialize();

void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);
void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);


#endif /* INC_SDHC_CARD_DRIVER_H_ */
