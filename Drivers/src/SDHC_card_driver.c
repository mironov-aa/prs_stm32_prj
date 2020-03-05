/*
 * SDHC_card_driver.c
 *
 *  Created on: Mar 5, 2020
 *      Author: mironov-aa
 */

#include "spi_driver.h"
#include "SDHC_card_driver.h"
#include "SDHC_card_encoding.h"

static void SendCmd(CommandIndex command, CommandArgument argument, uint8_t crc);
static inline ResponseR1 WaitForR1();


extern void g_Delay(uint32_t timeout);
extern void g_ErrorHandler(uint8_t errorCode);

uint8_t SdhcCardInitialize()
{
	const uint32_t maxRetries = 25;
	uint32_t retriesCounter = 0;
	ResponseR1 responseR1;
	ResponseR3 responseR3;
	ResponseR7 responseR7;
	CommandArgument argument;
	argument.value = 0;

	// 1. Power on
	g_Delay(48000); //~1ms
	SPI1_CsHigh();// CS high sd card for power up sequence
	for(uint8_t i = 0; i < 10; i++)
	{
		SPI1_TransmitReceive(0xFF); // MISO high ~80 clocks
	}
	// 2. Software reset
	SPI1_CsLow();

	SendCmd(CMD0, argument, 0x95);//go idle state
	WaitForR1();

	// 3. SDHC v.2 checkout
	argument.value = 0x0000001AA;//checkPattern = 0xAA
	SendCmd(CMD8,argument,0x86);
	responseR7.Field.R1 = WaitForR1();

	if(responseR7.Field.R1.Field.illegalCommand) //if SD v.1
	{
		g_ErrorHandler(5);
	}
	responseR7.bytes[3] = SPI1_TransmitReceive(0xFF);
	responseR7.bytes[2] = SPI1_TransmitReceive(0xFF);
	responseR7.bytes[1] = SPI1_TransmitReceive(0xFF);
	responseR7.bytes[0] = SPI1_TransmitReceive(0xFF);

	if((responseR7.Field.voltageAccepted != 0x1) || responseR7.Field.checkPattern != 0xAA)
	{
		g_ErrorHandler(5);
	}

	//4. Initialize card
	retriesCounter = 0;
	do
	{
		argument.value = 0;
		SendCmd(CMD55,argument,0x1);//Leading command of ACMD41 command
		WaitForR1();
		argument.value = 0x40000000;//bit HCS
		SendCmd(CMD41,argument,0x1);//ACMD41
		responseR1 = WaitForR1();
		retriesCounter++;
	}while((responseR1.Field.inIdleState) && (retriesCounter < maxRetries));//Sending ACMD41 until card be ready
	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(5);
	}

	//5. Read OCR
	argument.value = 0x0;
	SendCmd(CMD58,argument,0x1);
	responseR3.Field.R1 = WaitForR1();
	responseR3.bytes[3] = SPI1_TransmitReceive(0xFF);
	responseR3.bytes[2] = SPI1_TransmitReceive(0xFF);
	responseR3.bytes[1] = SPI1_TransmitReceive(0xFF);
	responseR3.bytes[0] = SPI1_TransmitReceive(0xFF);

	if(responseR3.Field.cardCapacityStatus != 1) //if SD SC(Standard Capacity)
	{
		g_ErrorHandler(5);
	}
	SPI1_CsHigh();
}

static void SendCmd(CommandIndex command, CommandArgument argument, uint8_t crc)
{
	SPI1_TransmitReceive(command);
	SPI1_TransmitReceive(argument.byte_array[3]);
	SPI1_TransmitReceive(argument.byte_array[2]);
	SPI1_TransmitReceive(argument.byte_array[1]);
	SPI1_TransmitReceive(argument.byte_array[0]);
	SPI1_TransmitReceive(crc); //crc must be with the end bit(0x1)!
}

static inline ResponseR1 WaitForR1()
{
	const uint32_t maxRetries = 25;
	ResponseR1 responseR1;
	uint32_t retriesCounter = 0;
	do
	{
		responseR1.value = SPI1_TransmitReceive(0xFF);
		retriesCounter++;
	}while((responseR1.Field.alwaysZero) && (retriesCounter < maxRetries));//wait for R1 response

	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(5);//Infinity loop if error occurred
	}

	return responseR1;
}
