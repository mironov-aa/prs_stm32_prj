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

#define __MEMORY_BARRIER __asm volatile( "dmb" ::: "memory" )


void g_Delay(uint32_t timeout);
void g_ErrorHandler(uint8_t errorCode);


#ifdef FREERTOS_DEBUG
extern volatile uint32_t g_highFrequencyTimerTicks = 0;
#endif
extern uint8_t g_dataStartPattern[512];








#endif /* INC_GLOBAL_H_ */
