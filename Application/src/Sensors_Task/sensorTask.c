/*
 *  sensorTask.c
 *
 *  Created: 4/3/2024 4:32:06 PM
 *  Author: wz, yx
 */ 

#include "FreeRTOS.h"
#include "task.h"
#include "sensorTask.h"

#include "WifiHandlerThread/WifiHandler.h"
#include "ClockTask/clock.h"
#include "voc/VOCcal.h"
#include "I2cDriver/I2cDriver.h"
#include "TEMPERATURE/SHTC3.h"
#include "AirQuality/SGP40.h"
#include "TFTdriver/tft.h"
#include "ADC/adc.h"
#include "PWM/pwm.h"

volatile bool buttonPressed = false;
volatile bool curtainClosed = true;


static void smart_open(void);
static void manual_open(uint8_t *openButton, uint8_t *closeButton);
static void timer_open(struct TimeInfo *open, struct TimeInfo *close);


void configure_button(void)
{
	struct port_config config_port_pin;
    port_get_config_defaults(&config_port_pin);
    config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(PIN_PB03, &config_port_pin);
}


/**
 * @fn		    void SHTC3Task(void *pvParameters)
 * @brief       Runs temperature measurement task in parallel
 * @details 	Runs temperature measurement task in parallel

 * @return		Returns nothing.
 * @note
 */

void sensorTask(void *pvParameters)	
{   
	
	configure_button();
	pwm_func_confg();
	adc_func_config();
	
	//LCD initialize
	configure_port_pins_LCD();
	LCD_init();
	LCD_menu();
	
	uint8_t buffer[64];
	uint8_t buffer1[64];
	// uint16_t buffer2[128];
	SerialConsoleWriteString("SHTC3 init start\r\n");
	
	int32_t res = SHTC3_Init();  //(buffer, 2);
	snprintf((char *) buffer1, sizeof(buffer1), "Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

	res = SGP40_Init();  //(buffer, 2);
	SerialConsoleWriteString("SGP40 init start\r\n");
	snprintf((char *) buffer1, sizeof(buffer1), "SGP40 Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

	//int32_t temperature = 0;
	//int32_t humidity = 0;
	//int32_t voc_raw = 0;
	//int32_t voc_index = 0;

	ModeType mode;
	
	struct ButtonDataPacket buttonData;
	buttonData.openButton = 0;
	buttonData.closeButton = 0;
	buttonData.mode = MANUAL;
	
	struct TimeInfo openTime;
	struct TimeInfo closeTime;
	
	xQueueButton = xQueueCreate(5, sizeof(struct ButtonDataPacket));
	xQueueSetTime = xQueueCreate(5, sizeof(struct TimeInfo));
	
	while(1)
	{   /*
		判断mode: if (pdPASS == xQueueReceive(xQueueModeInfo, &mode, 0))
		struct balaba{
			mode;
			openButton;
			closeButton;
		};
		*/
		if (pdPASS == xQueueReceive(xQueueButton, &buttonData, 0)){
			if(buttonData.mode == MANUAL || buttonData.mode == SMART || buttonData.mode == TIMER){
				mode = buttonData.mode;
			}
		}
	
		switch(mode) {
			
			case(SMART): {
				smart_open();
				break;
			}
			 //case(MANUAL): {
			 	//manual_open();
			 	//break;
			 //}
			case(TIMER): {
				timer_open(&openTime, &closeTime);
				break;
			}
		}
		manual_open(&buttonData.openButton, &buttonData.closeButton);

		//LCD_Sensor(temperature, humidity, voc_index, adc_res);

		//start_pwm();

		//vTaskDelay(1000);
		//stop_pwm();
		// Buzzer PB03
		//port_pin_set_output_level(PIN_PB03, false);
		vTaskDelay(1000);
	}
}

void open_curtain(void){
	start_pwm(1);
	port_pin_set_output_level(PIN_PB03, true);
	vTaskDelay(1000);
	port_pin_set_output_level(PIN_PB03, false);
	stop_pwm();
	// make sure the curtain is closed only once
	curtainClosed = false;
}

void close_curtain(void){
	start_pwm(0);
	port_pin_set_output_level(PIN_PB03, true);
	vTaskDelay(1000);
	port_pin_set_output_level(PIN_PB03, false);
	stop_pwm();
	// make sure the curtain is opened only once
	curtainClosed = true;
}


static void smart_open(void) {

	uint16_t light_res = getLightIntensity();
	if (light_res > 90 && curtainClosed)
	{
		open_curtain();
	}
	else if (light_res < 30 && !curtainClosed)
	{
		close_curtain();
	}
}

static void manual_open(uint8_t *openButton, uint8_t *closeButton) {
	/*
	if user press open button && curtainClosed:  Open the curtain
	if user press close button && !curtainClosed: Close the curtain
	*/
	if(*openButton == 1 && curtainClosed){
		open_curtain();
		*openButton = 0;
	}
	else if(*closeButton == 1 && !curtainClosed){
		close_curtain();
		*closeButton = 0;
	}
}


static void timer_open(struct TimeInfo *open, struct TimeInfo *close){
	// add a new Queue
	/*
	if current time = openTime && curtainClosed: Open the curtain
	if current time = closeTime && !curtainClosed: Closed the curtain
	*/
	struct TimeInfo receivedTime;
	if (pdPASS == xQueueReceive(xQueueSetTime, &receivedTime, 0)){
		if(receivedTime.type == TIME_INFO_SET_OPEN){
			open->minutes = receivedTime.minutes;
			open->hours = receivedTime.hours;
			SerialConsoleWriteString("open receive\r\n");
			if(curtainClosed) SerialConsoleWriteString("curtainClosed\r\n");
			else SerialConsoleWriteString("curtainNotClosed\r\n");
		}
		else if(receivedTime.type == TIME_INFO_SET_CLOSE){
			close->minutes = receivedTime.minutes;
			close->hours = receivedTime.hours;
			SerialConsoleWriteString("close receive\r\n");
			if(curtainClosed) SerialConsoleWriteString("curtainClosed\r\n");
			else SerialConsoleWriteString("curtainNotClosed\r\n");
		}
	}
	getTime(&receivedTime);
	if(receivedTime.hours == open->hours && receivedTime.minutes == open->minutes && curtainClosed){
		open_curtain();
		open->hours = 0;
		open->minutes = 0;
	}
	else if (receivedTime.hours == close->hours && receivedTime.minutes == close->minutes && !curtainClosed){
		close_curtain();
		close->hours = 0;
		close->minutes = 0;
	}
}

int getLightIntensity(void) {
	// read adc value
	uint16_t adc_res = adc_read_raw();

	// value ranges from 450 - 3000
	if (adc_res > 3000) return 100;
	if (adc_res < 450) return 0;

	// convert 450-3000 to 0-100
	uint16_t light_value = (adc_res - 450) * 100 / (3000 - 450);
	return light_value;
}

void getTemperatureHumidityVOC(uint16_t *temperature, uint16_t *humidity, uint16_t *VOC) {
    uint8_t buffer[64];
    uint8_t res = SHTC3_Read_Data(buffer, 4);  // Make sure to handle the result of this function to ensure data was read correctly

    if (res == 0) {
        // Calculate temperature
        *temperature = (((buffer[0] << 8) | buffer[1]) * 175 / 65536) - 45;

        // Calculate humidity
        *humidity = ((buffer[0] << 8) | buffer[1]) * 100 / 65536;

        res = SGP40_Read_Default_Data(buffer, 2);
        uint16_t voc_raw = (buffer[0] << 8) | buffer[1];

        int vocIndex; // Additional variable for VOC index
        VocAlgorithm_process2(voc_raw, &vocIndex);
        *VOC = (uint16_t) vocIndex;  // Assuming you still want to use the original VOC pointer for some reason

    } else {
        // Handle error conditions
        SerialConsoleWriteString("Error reading sensors\r\n");
    }
}










