/*
 *  clock.c
 *               
 *  Created: 4/22/2024 0:33:06 PM
 *  Author: T08
 */ 
#include "clock.h"
#include "FreeRTOS.h"
#include "task.h"

#include "WifiHandlerThread/WifiHandler.h"

void getTimeSinceBoot(TimeSinceBoot *time, TimeSinceBoot *tick) {
    TickType_t ticks = xTaskGetTickCount(); // Get the current tick count
    const TickType_t tickRate = configTICK_RATE_HZ; // Ticks per second, set in FreeRTOSConfig.h

    // Convert ticks to milliseconds
    uint32_t milliseconds = (ticks * 1000) / tickRate;

    // Calculate time components
    time->milliseconds = milliseconds % 1000;
    time->seconds = (milliseconds / 1000) % 60;
    time->minutes = ((milliseconds / (1000 * 60))+ tick->minutes) % 60;
    time->hours = ((milliseconds / (1000 * 60 * 60)) + tick->hours)% 24;
}


void clockTask(void *pvParameters) {
    TimeSinceBoot currentTickTime;
    TimeSinceBoot currentTime;
    TimeSinceBoot newTime;

    currentTickTime.hours = 0;
    currentTickTime.minutes = 0;
    currentTickTime.seconds = 0;
    currentTickTime.milliseconds = 0;

    currentTime.type = TIME_INFO_SEND;
    
    getTimeSinceBoot(&currentTime, &currentTickTime);
    uint32_t currentMinute = currentTime.minutes;
    while (1)
    {
        getTimeSinceBoot(&currentTime, &currentTickTime);
        currentMinute = currentTime.minutes;

        // update time info every 1 minute
        if(currentTime.minutes - currentMinute == 1){
            WifiAddTimeToQueue(&currentTime);
        }

        // if user adjust time
        if (pdPASS == xQueueReceive(xQueueTimeInfo, &newTime, 0)) {
            if(newTime.type == TIME_INFO_ADJUST){
                currentTickTime.hours = newTime.hours;
                currentTickTime.minutes = newTime.minutes;

                getTimeSinceBoot(&currentTime, &currentTickTime);
                WifiAddTimeToQueue(&currentTime);
            }
        }
        vTaskDelay(1);
    }
}
