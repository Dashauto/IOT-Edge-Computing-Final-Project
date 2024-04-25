/**
 * @file      sensorTask.c
 * @brief     File to handle HTTP Download and MQTT support
 * @author    T08
 * @date      4/3/2024 4:32:06 PM

 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/

#ifndef SENSOR_TASK_H_
#define SENSOR_TASK_H_

#include <string.h> // For memset
#include <asf.h>

/******************************************************************************
 * Variables
 ******************************************************************************/

#define Sensor_TASK_SIZE 700
#define SensorTask_PRIORITY		( configMAX_PRIORITIES - 3)

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

void sensorTask(void *pvParameters);
void configure_button(void);
void configure_button_callbacks(void);
void button_detection_callback(void);
int getLightIntensity(void);
void getTemperatureHumidityVOC(uint16_t *temperature, uint16_t *humidity, uint16_t *VOC);
void open_curtain(void);
void close_curtain(void);


#endif /* SENSOR_TASK_H_ */