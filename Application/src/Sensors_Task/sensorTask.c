/*
 *  sensorTask.c
 *
 *  Created: 4/3/2024 4:32:06 PM
 *  Author: wz, yx
 */ 

#include "FreeRTOS.h"
#include "task.h"
//#include "queue.h"
//#include "Global.h"
#include "sensorTask.h"

#include "voc/VOCcal.h"
#include "I2cDriver/I2cDriver.h"
#include "WifiHandlerThread/WifiHandler.h"
#include "TEMPERATURE/SHTC3.h"
#include "AirQuality/SGP40.h"
#include "TFTdriver/tft.h"
#include "ADC/adc.h"
#include "PWM/pwm.h"

volatile bool buttonPressed = false;
volatile bool curtainClosed = true;


static void smart_open(void);
static void manual_open(void);
static void timer_open(void);


// SETUP FOR EXTERNAL BUTTON INTERRUPT -- Used to send an MQTT Message

void configure_button(void)
{
	// struct extint_chan_conf config_extint_chan;
	// extint_chan_get_config_defaults(&config_extint_chan);
	// config_extint_chan.gpio_pin = BUTTON_0_EIC_PIN;
	// config_extint_chan.gpio_pin_mux = BUTTON_0_EIC_MUX;
	// config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	// config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;
	// extint_chan_set_config(BUTTON_0_EIC_LINE, &config_extint_chan);

	struct port_config config_port_pin;
    port_get_config_defaults(&config_port_pin);
    config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(PIN_PB03, &config_port_pin);
}

// void button_detection_callback(void)
// {
// 	buttonPressed = true;
// }

// void configure_button_callbacks(void)
// {
// 	extint_register_callback(button_detection_callback, BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
// 	extint_chan_enable_callback(BUTTON_0_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
// }

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
	//configure_button_callbacks();
	adc_func_config();
	
	//LCD initialize
	configure_port_pins_LCD();
	LCD_init();
	LCD_menu();
	
	uint8_t buffer[64];
	uint8_t buffer1[64];
	uint16_t buffer2[128];
	SerialConsoleWriteString("SHTC3 init start\r\n");
	
	int32_t res = SHTC3_Init();  //(buffer, 2);
	snprintf((char *) buffer1, sizeof(buffer1), "Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

	res = SGP40_Init();  //(buffer, 2);
	SerialConsoleWriteString("SGP40 init start\r\n");
	snprintf((char *) buffer1, sizeof(buffer1), "SGP40 Status of wakeup : %d\r\n", res);
	SerialConsoleWriteString(buffer1);

	int32_t temperature = 0;
	int32_t humidity = 0;
	int32_t voc_raw = 0;
	int32_t voc_index = 0;
	
	ModeType mode = SMART;
	
	while(1)
	{    
		switch(mode){
			
			case(SMART): {
				smart_open();
				break;
			}
			case(MANUAL): {
				manual_open();
				break;
			}
			case(TIMER): {
				timer_open();
				break;
			}
			default:
                mode = SMART;
                break;
		}

		//LCD_Sensor(temperature, humidity, voc_index, adc_res);

		//start_pwm();

		//vTaskDelay(1000);
		//stop_pwm();
		// Buzzer PB03
		//port_pin_set_output_level(PIN_PB03, false);
		vTaskDelay(1000);
	}
}




static void smart_open(void) {

	uint16_t light_res = getLightIntensity();
	if (light_res > 90 && curtainClosed)
	{
		start_pwm(1);
		port_pin_set_output_level(PIN_PB03, true);
		vTaskDelay(1000);
		port_pin_set_output_level(PIN_PB03, false);
		stop_pwm();
		// make sure the curtain is closed only once
		curtainClosed = false;
	}
	else if (light_res < 30 && !curtainClosed)
	{
		start_pwm(0);
		port_pin_set_output_level(PIN_PB03, true);
		vTaskDelay(1000);
		port_pin_set_output_level(PIN_PB03, false);
		stop_pwm();
		// make sure the curtain is opened only once
		curtainClosed = true;
	}
}

static void manual_open(void){}
static void timer_open(void){}

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
    uint8_t res = SHTC3_Read_Data(buffer, 2);  // Make sure to handle the result of this function to ensure data was read correctly

    if (res == 0) {
        // Calculate temperature
        *temperature = (((buffer[0] << 8) | buffer[1]) * 175 / 65536) - 45;

        // Calculate humidity
        *humidity = ((buffer[2] << 8) | buffer[3]) * 100 / 65536;

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










