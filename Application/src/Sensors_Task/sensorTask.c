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
#include "voc/sensirion_voc_algorithm.h"
#include "voc/VOCcal.h"
#include "I2cDriver/I2cDriver.h"
#include "WifiHandlerThread/WifiHandler.h"
#include "TEMPERATURE/SHTC3.h"
#include "AirQuality/SGP40.h"
#include "TFTdriver/tft.h"
#include "ADC/adc.h"
#include "PWM/pwm.h"

volatile bool buttonPressed = false;

static VocAlgorithmParams voc_algorithm_params;

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
	
		
	while(1)
	{    
		buttonPressed = button_status();
		if(buttonPressed) {
			// Buzzer PB03
			// port_pin_set_output_level(PIN_PB03, true);

			buttonPressed = false;
			res = SHTC3_Read_Data(buffer, 2);
			/**
			 * formulas for conversion of the sensor signals, optimized for fixed point
			 * algebra:
			 * Temperature = 175 * S_T / 2^16 - 45
			 * Relative Humidity = 100 * S_RH / 2^16
			 */
			temperature = (((buffer[0] << 8 | buffer[1]) * 175) / 65536 ) - 45;
			humidity = ((buffer[0] << 8) | buffer[1]) * 100 / 65536;

			snprintf((char *) buffer1, sizeof(buffer1), "Temp : %d Humi: %d\r\n",temperature, humidity);
			SerialConsoleWriteString(buffer1);

			vTaskDelay(1);

			res = SGP40_Read_Default_Data(buffer, 2);
			voc_raw = (buffer[0] << 8) | buffer[1];
			snprintf((char *) buffer1, sizeof(buffer1), "voc_raw : %d\r\n",voc_raw);
			SerialConsoleWriteString(buffer1);
			//VocAlgorithm_init(&voc_algorithm_params);
			//VocAlgorithm_process(&voc_algorithm_params, voc_raw, &voc_index);
			VocAlgorithm_process2(voc_raw, &voc_index);


			snprintf((char *) buffer1, sizeof(buffer1), "VOC Index : %d\r\n",voc_index);
			SerialConsoleWriteString(buffer1);

			// value ranges from 450 - 3000
			uint16_t adc_res = adc_read_raw();
			 if(adc_res != -1){
			 	snprintf((char *) buffer1, sizeof(buffer1), "ADC Value : %d\r\n", adc_res);
			 	SerialConsoleWriteString(buffer1);
			 }

			LCD_Sensor(temperature, humidity, voc_index, adc_res);

			//start_pwm();

			vTaskDelay(1000);
			//stop_pwm();
			// Buzzer PB03
			//port_pin_set_output_level(PIN_PB03, false);
		}
	}
}


