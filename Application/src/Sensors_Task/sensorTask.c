/**
 * @file      sensorTask.c
 * @brief     File to handle HTTP Download and MQTT support
 * @author    T08
 * @date      4/3/2024 4:32:06 PM

 ******************************************************************************/

/******************************************************************************
 * Includes
 ******************************************************************************/
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

/******************************************************************************
 * Variables
 ******************************************************************************/

volatile bool buttonPressed = false;
volatile bool curtainClosed = true;

/******************************************************************************
 * Forward Declarations
 ******************************************************************************/

static void smart_open(void);
static void manual_open(uint8_t *openButton, uint8_t *closeButton);
static void timer_open(struct TimeInfo *open, struct TimeInfo *close);

/**
 * @fn		    void SHTC3Task(void *pvParameters)
 * @brief       Runs temperature measurement task in parallel
 * @details 	Runs temperature measurement task in parallel

 * @return		Returns nothing.
 * @note
 */

void sensorTask(void *pvParameters)	
{   
	pwm_func_confg();
	adc_func_config();
	
	//LCD initialize
	configure_port_pins_LCD();
	LCD_init();
	LCD_menu();
	
	uint8_t buffer[64];
	uint8_t buffer1[64];
	SerialConsoleWriteString("SHTC3 init start\r\n");
	
	int32_t res = SHTC3_Init();  //(buffer, 2);
	snprintf((char *) buffer1, sizeof(buffer1), "Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

	res = SGP40_Init();  //(buffer, 2);
	SerialConsoleWriteString("SGP40 init start\r\n");
	snprintf((char *) buffer1, sizeof(buffer1), "SGP40 Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

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
	{   
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

		vTaskDelay(1000);
	}
}

/*
 * @fn		    void open_curtain(void)
 * @brief       Open the curtain
 * @details 	Set the motor to open the curtain, buzzer will sound for 1 second

 * @return		Returns nothing.
 * @note
*/
void open_curtain(void){
	start_pwm(1);
	port_pin_set_output_level(PIN_PB03, true);
	vTaskDelay(1000);
	port_pin_set_output_level(PIN_PB03, false);
	stop_pwm();
	// make sure the curtain is closed only once
	curtainClosed = false;
}

/*
 * @fn		    void close_curtain(void)
 * @brief       Close the curtain
 * @details 	Set the motor to close the curtain, buzzer will sound for 1 second

 * @return		Returns nothing.
 * @note
*/
void close_curtain(void){
	start_pwm(0);
	port_pin_set_output_level(PIN_PB03, true);
	vTaskDelay(1000);
	port_pin_set_output_level(PIN_PB03, false);
	stop_pwm();
	// make sure the curtain is opened only once
	curtainClosed = true;
}

/*
 * @fn		    void smart_open(void)
 * @brief       Open or close the curtain based on the light intensity
 * @details 	Open the curtain if the light intensity is greater than 90, close the curtain if the light intensity is less than 30

 * @return		Returns nothing.
 * @note
*/
static void smart_open(void) 
{
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

/*
 * @fn		    void manual_open(uint8_t *openButton, uint8_t *closeButton)
 * @brief       Open or close the curtain based on the user input
 * @details 	Open the curtain if the open button is pressed and the curtain is closed, close the curtain if the close button is pressed and the curtain is opened

 * @return		Returns nothing.
 * @note
*/
static void manual_open(uint8_t *openButton, uint8_t *closeButton) 
{
	/*
	if user press open button && curtainClosed:  Open the curtain
	if user press close button && !curtainClosed: Close the curtain
	*/
	if(*openButton == 1 && curtainClosed)
	{
		open_curtain();
		*openButton = 0;
	}
	else if(*closeButton == 1 && !curtainClosed)
	{
		close_curtain();
		*closeButton = 0;
	}
}

/*
 * @fn		    void timer_open(struct TimeInfo *open, struct TimeInfo *close)
 * @brief       Open or close the curtain based on the time set by the user
 * @details 	Open the curtain if the current time is equal to the open time 
 				set by the user and the curtain is closed, close the curtain 
				if the current time is equal to the close time set by the user 
				and the curtain is opened

 * @return		Returns nothing.
 * @note
*/
static void timer_open(struct TimeInfo *open, struct TimeInfo *close){
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

/**
 * @fn		    uint16_t getLightIntensity(void)
 * @brief       Get the light intensity
 * @details 	Get the light intensity from the light sensor

 * @return		Returns the light intensity.
 * @note
*/
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

/**
 * @fn		    void getTemperatureHumidityVOC(uint16_t *temperature, uint16_t *humidity, uint16_t *VOC)
 * @brief       Get the temperature, humidity and VOC
 * @details 	Get the temperature, humidity and VOC from the sensors

 * @return		Returns nothing.
 * @note
*/
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










