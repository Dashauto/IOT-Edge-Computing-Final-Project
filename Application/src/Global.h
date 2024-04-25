/*
 * Global.h
 *
 * Created: 2024/3/9 19:53:54
 *  Author: YINING
 */ 


#ifndef GLOBAL_H
#define GLOBAL_H

#include "FreeRTOS.h"
#include "queue.h"
extern QueueHandle_t xQueueTimeInfo;
extern QueueHandle_t xQueueTimeAdjInfo;        ///< Queue to send and receive time to the cloud
extern QueueHandle_t xQueueButton;
extern QueueHandle_t xQueueSetTime;
#endif // GLOBAL_H
