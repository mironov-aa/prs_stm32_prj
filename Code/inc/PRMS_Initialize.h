/*
 * PRMS_Initialize.h
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */

#ifndef INC_PRMS_INITIALIZE_H_
#define INC_PRMS_INITIALIZE_H_

#include <stdint.h>
#include "stm32f0xx.h"

static uint8_t __IS_HSE_START__ = 0; //< Set if HSE don't start after system_stm32f0xx.c::SetSysClock();

void PrmsInitialize(void);//	Inizialize all
void InterruptInitialize(void);// Interrupts priority, NVIC & EXTI
void GpioInitialize(void);



#endif /* INC_PRMS_INITIALIZE_H_ */
