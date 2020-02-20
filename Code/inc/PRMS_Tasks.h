/*
 * PRMS_Tasks.h
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#ifndef INC_PRMS_TASKS_H_
#define INC_PRMS_TASKS_H_

TaskHandle_t __BUTTON_HANDLER__ = NULL;
TaskHandle_t __LED2_HANDLER__ = NULL;
TaskHandle_t __LED1_HANDLER__ = NULL;


//PRMS tasks
void vLed1Task(void* argument);
void vLed2Task(void* argument);
void vButtonTask(void* argument);

//Default FreeRTOS tasks
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char* pcTaskName);

#endif /* INC_PRMS_TASKS_H_ */
