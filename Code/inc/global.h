/*
 * global.h
 *
 *  Created on: 8 апр. 2020 г.
 *      Author: mironov-aa
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"
#include "stm32f0xx.h"
#include "stdint.h"

#define __MEMORY_BARRIER __asm volatile( "dmb" ::: "memory" )


void g_Delay(uint32_t timeout);
void g_ErrorHandler(uint8_t errorCode);

typedef union
{
	uint8_t byteArray[43];
	struct
	{
		uint8_t word0[5];
		uint8_t timeValue[6];
		uint8_t newLine0;
		uint8_t word2[5];
		uint8_t dataValue[6];
		uint8_t newLine2;
		uint8_t word3[4];
		uint8_t setTime;
		uint8_t newLine3;
		uint8_t word4[4];
		uint8_t numberOfCounters[2];
		uint8_t newLine4;
		uint8_t word5[4];
		uint8_t periodMs[2];
	}Field;
}USettings;

struct g_settings
{
	uint8_t numberOfCounters;
	uint8_t periodMs;
}g_settings;

extern volatile uint8_t g_settings_numberOfCounters;
extern volatile uint8_t g_settings_period;





#ifdef FREERTOS_DEBUG
extern volatile uint32_t g_highFrequencyTimerTicks;
#endif








#endif /* INC_GLOBAL_H_ */
