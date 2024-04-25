/*
 *  clock.c
 *               
 *  Created: 4/22/2024 0:33:06 PM
 *  Author: T08
 */ 
#include "clock.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Global.h"
#include "SerialConsole.h"
#include "Sensors_Task/sensorTask.h"
#include "TFTdriver/tft.h"

TickType_t ticknum = 0;
struct TimeInfo currentTime;
struct TimeInfo newTime;


/**
 * @fn		    void getTimeSinceBoot(TimeInfo *time, TimeInfo *tick)
 * 
 * @brief       Get the time since the system booted, adjusted by the time the system was set to
 * @param       time: TimeInfo struct to store the time since boot
 * @param       tick: TimeInfo struct to store the time the system was set to
 * 
*/
void getTimeSinceBoot(struct TimeInfo *time, struct TimeInfo *tick) {
    TickType_t ticks = xTaskGetTickCount() - ticknum; // Get the current tick count

    // Calculate time components
    time->milliseconds = ticks % 1000;
    time->seconds = (ticks / 1000) % 60;
    time->minutes = ((ticks / (1000 * 60))+ tick->minutes) % 60;
    time->hours = ((ticks / (1000 * 60 * 60)) + tick->hours)% 24;
}

/**
 * @fn		    void clockTask(void *pvParameters)
 * 
 * @brief       Runs clock task in parallel
 * 
*/
void clockTask(void *pvParameters) {
    
    bool firstRun = true;
    struct SensorDataPacket sensorData;
    uint8_t buffer[64];

    xQueueTimeAdjInfo = xQueueCreate(5, sizeof(struct TimeInfo));


    newTime.hours = 0;
    newTime.minutes = 0;
    newTime.seconds = 0;
    newTime.milliseconds = 0;

    currentTime.type = TIME_INFO_SEND;
    
    getTimeSinceBoot(&currentTime, &newTime);

    uint32_t currentMinute = currentTime.minutes;

    SerialConsoleWriteString("Start clock task\r\n");

    while (1)
    {
        //volatile UBaseType_t uxHighWaterMark;
	    //uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );

        getTimeSinceBoot(&currentTime, &newTime);

        // print all default messages when power is on for 5s.
        if(firstRun && currentTime.seconds == 5){
            WifiAddTimeToQueue(&currentTime);

            sensorData.light_intensity = getLightIntensity();
            getTemperatureHumidityVOC(&sensorData.temperature, &sensorData.humidity, &sensorData.VOCvalue);
            wifiAddSensorDataToQueue(&sensorData);
            LCD_Sensor(sensorData.temperature, sensorData.humidity, sensorData.VOCvalue, 
                        sensorData.light_intensity, currentTime.hours, currentTime.minutes);
            firstRun = false;
        }
        

         // update time info every 1 minute
         if(currentTime.minutes - currentMinute >= 10){
            // send current time to web
            WifiAddTimeToQueue(&currentTime);
            
            currentMinute = currentTime.minutes;

            // send sensor data to web
            sensorData.light_intensity = getLightIntensity();
            getTemperatureHumidityVOC(&sensorData.temperature, &sensorData.humidity, &sensorData.VOCvalue);
            wifiAddSensorDataToQueue(&sensorData);

            // display on LCD screen
			LCD_Sensor(sensorData.temperature, sensorData.humidity, sensorData.VOCvalue, 
                        sensorData.light_intensity, currentTime.hours, currentTime.minutes);
         }

        // if user adjust time
        if (pdPASS == xQueueReceive(xQueueTimeAdjInfo, &newTime, 0)) {
            if(newTime.type == TIME_INFO_ADJUST) {
                SerialConsoleWriteString("\r\n adjust time...\r\n");
				//snprintf(buffer, 63, "{\"hour\":%d, \"min\": %d, \"sec\": %d}", newTime.hours, newTime.minutes, newTime.seconds);
				//SerialConsoleWriteString(buffer);
                ticknum = xTaskGetTickCount();
                getTimeSinceBoot(&currentTime, &newTime);
                WifiAddTimeToQueue(&currentTime);
            }
        }
        // Display stack usage
        //uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        //snprintf(buffer, sizeof(buffer), "Stack high water mark: %lu words left\r\n", uxHighWaterMark);
        //SerialConsoleWriteString(buffer);

        vTaskDelay(100);
    }
}

void getTime(struct TimeInfo *time){
	getTimeSinceBoot(time, &newTime);
}
