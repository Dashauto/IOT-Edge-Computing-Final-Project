/*
 *  clock.h
 *               
 *  Created: 4/22/2024 0:33:08 PM
 *  Author: T08
 */ 
#ifndef CLOCK_TASK_H_
#define CLOCK_TASK_H_

#include <string.h>
#include <asf.h>


#define Clock_TASK_SIZE 700
#define ClockTask_PRIORITY		( configMAX_PRIORITIES - 3 )


void clockTask(void *pvParameters);

#endif /* CLOCK_TASK_H_ */