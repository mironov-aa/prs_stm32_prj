/*
 * sdhc_card_driver.c
 *
 *  Created on: Mar 5, 2020
 *  Author: mironov-aa
 */

#include "global.h"
#include "spi_driver.h"
#include "sdhc_card_encoding.h"
#include "sdhc_card_driver.h"


#define _MAX_RETRIES_ ( (uint32_t)0xFFFF )

static void SendCmd(CommandIndex command, 
                    CommandArgument argument, uint8_t crc);
static inline ResponseR1 WaitForR1();
static inline ResponseR2 WaitForR2();
static inline ResponseR7 WaitForR7();//TODO:: !!!!
static inline DataResponse WaitForDataResponse();
static inline void WaitForStartDataToken();

uint8_t SdhcCardInitialize()
{
    uint32_t retriesCounter = 0;
    ResponseR1 responseR1;
    ResponseR3 responseR3;
    ResponseR7 responseR7;
    CommandArgument argument;
    argument.value = 0;

    // 1. Power on
    for(uint8_t i = 0; i < 3; i++)
    {
        G_Delay(480000); // > 10ms
        SPI1_CsHigh();// CS high sd card for power up sequence
        for(uint8_t i = 0; i < 23; i++)
        {
            SPI1_TransmitReceive(0xFF); // MISO high >80 clocks
        }
        G_Delay(480000); // > 10ms
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
        G_ErrorHandler(G_SDHC_ERROR);
    }
    responseR7.bytes[3] = SPI1_TransmitReceive(0xFF);
    responseR7.bytes[2] = SPI1_TransmitReceive(0xFF);
    responseR7.bytes[1] = SPI1_TransmitReceive(0xFF);
    responseR7.bytes[0] = SPI1_TransmitReceive(0xFF);

    if((responseR7.Field.voltageAccepted != 0x1) || 
        responseR7.Field.checkPattern != 0xAA)
    {
        G_ErrorHandler(G_SDHC_ERROR);
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
    }while((responseR1.Field.inIdleState) && 
           (retriesCounter < _MAX_RETRIES_));//Sending ACMD41 until card be rdy
    if(retriesCounter == _MAX_RETRIES_)
    {
        G_ErrorHandler(G_SDHC_ERROR);
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
        G_ErrorHandler(G_SDHC_ERROR);
    }
    SPI1_CsHigh();
}

void SdhcCardReadBlock(uint8_t* bufferOut, uint32_t blockIndex)
{
    CommandArgument argument;
    argument.value = blockIndex;

    SPI1_CsLow();
    SendCmd(CMD17,argument,0x1);
    WaitForR1();
    WaitForStartDataToken();
    SPI1_OnlyReceiveBlockDMA(bufferOut);
    SPI1_TransmitReceive(0xFF);//two bytes
    SPI1_TransmitReceive(0xFF);//crc 16
    SPI1_CsHigh();
}

void SdhcCardWriteBlock(uint8_t* bufferIn, uint32_t blockIndex)
{
    CommandArgument argument;
    DataResponse dataResponse;
    ResponseR2 responseR2;

    argument.value = blockIndex;

    SPI1_CsLow();
    SendCmd(CMD24,argument,0x1);
    WaitForR1();

    SPI1_TransmitReceive(START_BLOCK_TOKEN);
    SPI1_OnlyTransmitBlockDMA(bufferIn);

    dataResponse = WaitForDataResponse();
    while(SPI1_TransmitReceive(0xFF) != 0xFF);

    if(dataResponse.Field.status != 0b010)// status != ok
    {
        argument.value = 0;
        SendCmd(CMD13, argument, 0x1);
        responseR2 = WaitForR2();
        G_ErrorHandler(G_SDHC_ERROR);
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
    ResponseR1 responseR1;
    uint32_t retriesCounter = 0;
    do
    {
        responseR1.value = SPI1_TransmitReceive(0xFF);
        retriesCounter++;
    }while((responseR1.Field.alwaysZero) && 
           (retriesCounter < _MAX_RETRIES_));//wait for R1 response

    if(retriesCounter == _MAX_RETRIES_)
    {
        G_ErrorHandler(G_SDHC_ERROR);//Infinity loop if error occurred
    }
    return responseR1;
}

static inline ResponseR2 WaitForR2()
{
    ResponseR2 responseR2;
    uint32_t retriesCounter = 0;
    do
    {
        responseR2.bytes[1] = SPI1_TransmitReceive(0xFF);
        retriesCounter++;
    }while((responseR2.Field.alwaysZero) && 
           (retriesCounter < _MAX_RETRIES_));//wait for R1 response
           
    responseR2.bytes[0] = SPI1_TransmitReceive(0xFF);
    if(retriesCounter == _MAX_RETRIES_)
    {
        G_ErrorHandler(G_SDHC_ERROR);//Infinity loop if error occurred
    }
    return responseR2;
}

static inline DataResponse WaitForDataResponse()
{
    DataResponse dataResponse;
    uint32_t retriesCounter = 0;
    
    do
    {
        dataResponse.value = SPI1_TransmitReceive(0xFF); //wait for R1 response
        retriesCounter++;
    }while((dataResponse.Field.allwaysZero) &&
           (retriesCounter < _MAX_RETRIES_));

    if(retriesCounter == _MAX_RETRIES_)
    {
        G_ErrorHandler(G_SDHC_ERROR);//Infinity loop if error occurred
    }

    return dataResponse;
}

static inline void WaitForStartDataToken()
{
    uint32_t retriesCounter = 0;
    uint8_t receivedData = 0;
    do
    {
        receivedData = SPI1_TransmitReceive(0xFF); //wait for start token
        retriesCounter++;
    }while((receivedData != START_BLOCK_TOKEN) && 
           (retriesCounter < _MAX_RETRIES_));

    if(retriesCounter == _MAX_RETRIES_)
    {
        G_ErrorHandler(G_SDHC_ERROR);//Infinity loop if error occurred
    }
}
