/*
 *  sensorTask.h
 *               
 *  Created: 4/3/2024 4:32:06 PM
 *  Author: T08
 */ 


#ifndef SENSOR_TASK_H_
#define SENSOR_TASK_H_

#include <string.h> // For memset
#include <asf.h>


#define Sensor_TASK_SIZE 500
#define SensorTask_PRIORITY		( configMAX_PRIORITIES - 4 )

void sensorTask(void *pvParameters);
void configure_button(void);
void configure_button_callbacks(void);
void button_detection_callback(void);



#endif /* SENSOR_TASK_H_ */