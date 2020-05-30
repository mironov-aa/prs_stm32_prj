/*
 * SDHC_card_encoding.h
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */
#ifndef INC_SDHC_CARD_ENCODING_H_
#define INC_SDHC_CARD_ENCODING_H_

typedef enum
{
	CMD0 = 0x40,
	CMD1, // brings card out of idle state
	CMD2, // not used in SPI mode
	CMD3, // not used in SPI mode
	CMD4, // not used in SPI mode
	CMD5, // Reserved
	CMD6, // Reserved
	CMD7, // not used in SPI mode
	CMD8, // Reserved
	CMD9, // ask card to send card speficic data (CSD)
	CMD10, // ask card to send card identification (CID)
	CMD11, // not used in SPI mode
	CMD12, // stop transmission on multiple block read
	CMD13, // ask the card to send it's status register
	CMD14, // Reserved
	CMD15, // not used in SPI mode
	CMD16, // sets the block length used by the memory card
	CMD17, // read single block
	CMD18, // read multiple block
	CMD19, // Reserved
	CMD20, // not used in SPI mode
	CMD21, // Reserved
	CMD22, // Reserved
	CMD23, // Reserved
	CMD24, // writes a single block
	CMD25, // writes multiple blocks
	CMD26, // not used in SPI mode
	CMD27, // change the bits in CSD
	CMD28, // sets the write protection bit
	CMD29, // clears the write protection bit
	CMD30, // checks the write protection bit
	CMD31, // Reserved
	CMD32, // Sets the address of the first sector of the erase group
	CMD33, // Sets the address of the last sector of the erase group
	CMD34, // removes a sector from the selected group
	CMD35, // Sets the address of the first group
	CMD36, // Sets the address of the last erase group
	CMD37, // removes a group from the selected section
	CMD38, // erase all selected groups
	CMD39, // not used in SPI mode
	CMD40, // not used in SPI mode
	CMD41, // Reserved
	CMD42, // locks a block
	CMD55 = 0x77,
	CMD58 = 0x7A, // reads the OCR register
	CMD59, // turns CRC off
}CommandIndex; //Command index with start bit & transmission bit

typedef enum
{
	START_BLOCK_TOKEN = 0xFE,
}Tokens;

typedef union
{
	uint32_t value;
	uint8_t byte_array[4];
}CommandArgument;

typedef union
{
	uint8_t value;
	struct
	{
		uint8_t inIdleState : 1;
		uint8_t eraseReset : 1;
		uint8_t illegalCommand : 1;
		uint8_t crcError : 1;
		uint8_t eraseSequenceError : 1;
		uint8_t addressError : 1;
		uint8_t parameterError : 1;
		uint8_t alwaysZero : 1;
	}Field;
}ResponseR1;

typedef union
{
	uint8_t bytes[2];
	struct
	{
		uint8_t lockedCard : 1;
		uint8_t writeProtect : 1;
		uint8_t error : 1;
		uint8_t ccError : 1;
		uint8_t cardEccFailed : 1;
		uint8_t writeProtectViolation : 1;
		uint8_t eraseParam : 1;
		uint8_t outOfRange : 1;
		uint8_t inIdleState : 1;
		uint8_t eraseReset : 1;
		uint8_t illegalCommand : 1;
		uint8_t crcError : 1;
		uint8_t eraseSequenceError : 1;
		uint8_t addressError : 1;
		uint8_t parameterError : 1;
		uint8_t alwaysZero : 1;
	}Field;
}ResponseR2;

typedef union
{
	uint8_t  bytes[5];
	struct
	{
		uint16_t reserve0 : 15;
		uint8_t supportedVoltage1 : 1;//2,7 - 2,8 V
		uint8_t supportedVoltage2 : 1;//2,8 - 3,9 V
		uint8_t supportedVoltage3 : 1;//2,9 - 3,0 V
		uint8_t supportedVoltage4 : 1;//3,0 - 3,1 V
		uint8_t supportedVoltage5 : 1;//3,1 - 3,2 V
		uint8_t supportedVoltage6 : 1;//3,2 - 3,3 V
		uint8_t supportedVoltage7 : 1;//3,3 - 3,4 V
		uint8_t supportedVoltage8 : 1;//3,4 - 3,5 V
		uint8_t supportedVoltage9 : 1;//3,5 - 3,6 V
		uint8_t s18a : 1;
		uint8_t reserve1 : 4;
		uint8_t uhsCardStatus : 1;
		uint8_t cardCapacityStatus : 1;
		uint8_t cardPowerUpStatus : 1;
		ResponseR1 R1;
	}Field;
}ResponseR3;


typedef union
{
	uint8_t bytes[5];
	struct
	{
		uint8_t checkPattern;
		uint8_t voltageAccepted : 4;
		uint16_t reserve : 16;
		uint8_t commandVersion : 4;
		ResponseR1 R1;
	}Field;
}ResponseR7;

typedef union
{
	uint8_t value;
	struct
	{
		uint8_t allwaysOne : 1;
		uint8_t status : 3;
		uint8_t allwaysZero : 1;
		uint8_t reserve : 3;
	}Field;
}DataResponse;


#endif /* INC_SDHC_CARD_ENCODING_H_ */
