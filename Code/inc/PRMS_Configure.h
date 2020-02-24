/*
 * PRMS_Configure.h
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */

#ifndef INC_PRMS_CONFIGURE_H_
#define INC_PRMS_CONFIGURE_H_


void ConfigurePrms(void);
static inline void ConfigureGpio(void);
static inline void CongigureInterrupts(void); // Interrupts priority, NVIC & EXTI
static inline void ConfigureSpi1(void);
static inline void ConfigureSpi2(void);



#endif /* INC_PRMS_INITIALIZE_H_ */
