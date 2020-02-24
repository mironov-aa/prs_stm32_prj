/*
 * PRMS_Tasks.h
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#ifndef INC_PRMS_TASKS_H_
#define INC_PRMS_TASKS_H_

//PRMS tasks
void vButtonTask(void* argument);
void vFpgaTask(void* argument);
void vLedTask(void* argument);


//Default FreeRTOS tasks
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName);

#endif /* INC_PRMS_TASKS_H_ */
