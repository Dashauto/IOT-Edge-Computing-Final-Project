/*
 *  clock.h
 *               
 *  Created: 4/22/2024 0:33:08 PM
 *  Author: T08
 */ 
#ifndef CLOCK_H_
#define CLOCK_H_

#include <string.h>
#include <asf.h>
#include "WifiHandlerThread/WifiHandler.h"


#define Clock_TASK_SIZE 300
#define ClockTask_PRIORITY		( configMAX_PRIORITIES - 5 )


void clockTask(void *pvParameters);
void getTime(struct TimeInfo *time);
void getTimeSinceBoot(struct TimeInfo *time, struct TimeInfo *tick);


#endif /* CLOCK_H_ */