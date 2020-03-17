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
static inline ResponseR2 WaitForR2();
static inline ResponseR7 WaitForR7();//TODO:: !!!!
static inline DataResponse WaitForDataResponse();
static inline void WaitForStartDataToken();


extern void g_Delay(uint32_t timeout);
extern void g_ErrorHandler(uint8_t errorCode);
extern volatile uint8_t g_dataBuffer[512];

uint8_t SdhcCardInitialize()
{
	const uint32_t maxRetries = 100;
	uint32_t retriesCounter = 0;
	ResponseR1 responseR1;
	ResponseR3 responseR3;
	ResponseR7 responseR7;
	CommandArgument argument;
	argument.value = 0;

	// 1. Power on
	for(int i = 0; i < 3; i++)
	{
		g_Delay(480000); // > 10ms
		SPI1_CsHigh();// CS high sd card for power up sequence
		for(uint8_t i = 0; i < 23; i++)
		{
			SPI1_TransmitReceive(0xFF); // MISO high >80 clocks
		}
		g_Delay(480000); // > 10ms
		// 2. Software reset
		SPI1_CsLow();

		SendCmd(CMD0, argument, 0x95);//go idle state
		WaitForR1();
	}

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
		g_ErrorHandler(6);
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
		g_ErrorHandler(7);
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
		g_ErrorHandler(8);
	}
	SPI1_CsHigh();
}

void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index)
{
	CommandArgument argument;
	argument.value = block_index;

	SPI1_CsLow();
	SendCmd(CMD17,argument,0x1);
	WaitForR1();
	WaitForStartDataToken();
	for(uint32_t i = 0; i < 512; i++)
	{
		buffer_out[i] = SPI1_TransmitReceive(0xFF);
	}
	SPI1_TransmitReceive(0xFF);//two bytes
	SPI1_TransmitReceive(0xFF);//crc 16
	SPI1_CsHigh();
}

void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index)
{
	CommandArgument argument;
	DataResponse dataResponse;
	ResponseR2 responseR2;
	const uint32_t busyTimeout = 10;
	uint32_t busyTimeoutCounter = 0;

	argument.value = block_index;

	SPI1_CsLow();
	SendCmd(CMD24,argument,0x1);
	WaitForR1();

	SPI1_TransmitReceive(START_BLOCK_TOKEN);
	SPI1_OnlyTransmitBlockDMA(buffer_in);
	/*
	for(uint32_t i = 0; i < 512; i++)
	{
		SPI1_TransmitReceive(buffer_in[i]);//TODO:: Need DMA!
	}*/

	dataResponse = WaitForDataResponse();
	while(SPI1_TransmitReceive(0xFF) != 0xFF);

	if(dataResponse.Field.status != 0b010)// status != ok
	{

		argument.value = 0;
		SendCmd(CMD13, argument, 0x1);
		responseR2 = WaitForR2();
		g_ErrorHandler(9);
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
	const uint32_t maxRetries = 50;
	ResponseR1 responseR1;
	uint32_t retriesCounter = 0;
	do
	{
		responseR1.value = SPI1_TransmitReceive(0xFF);
		retriesCounter++;
	}while((responseR1.Field.alwaysZero) && (retriesCounter < maxRetries));//wait for R1 response

	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(1);//Infinity loop if error occurred
	}

	return responseR1;
}

static inline ResponseR2 WaitForR2()
{
	const uint32_t maxRetries = 50;
	ResponseR2 responseR2;
	uint32_t retriesCounter = 0;
	do
	{
		responseR2.bytes[1] = SPI1_TransmitReceive(0xFF);
		retriesCounter++;
	}while((responseR2.Field.alwaysZero) && (retriesCounter < maxRetries));//wait for R1 response
	responseR2.bytes[0] = SPI1_TransmitReceive(0xFF);
	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(2);//Infinity loop if error occurred
	}
	return responseR2;
}

static inline DataResponse WaitForDataResponse()
{
	const uint32_t maxRetries = 50;
	DataResponse dataResponse;
	uint32_t retriesCounter = 0;
	do
	{
		dataResponse.value = SPI1_TransmitReceive(0xFF);
		retriesCounter++;
	}while((dataResponse.Field.allwaysZero) && (retriesCounter < maxRetries));//wait for R1 response

	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(3);//Infinity loop if error occurred
	}

	return dataResponse;
}



static inline void WaitForStartDataToken()
{
	const uint32_t maxRetries = 0xFFFF;
	uint32_t retriesCounter = 0;
	uint8_t received_data = 0;
	do
	{
		received_data = SPI1_TransmitReceive(0xFF);
		retriesCounter++;
	}while((received_data != START_BLOCK_TOKEN) && (retriesCounter < maxRetries));//wait for R1 response

	if(retriesCounter == maxRetries)
	{
		g_ErrorHandler(4);//Infinity loop if error occurred
	}
}
