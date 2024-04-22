/*
 * ButtonTask.c
 *
 * Created: 2024/3/10 20:11:20
 *  Author: YINING
 */ 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ButtonTask.h"
#include "asf.h"
#include "Global.h"
#include "SerialConsole.h"


/******************************************************************************
 * Defines
 ******************************************************************************/

/******************************************************************************
 * Variables
 ******************************************************************************/
static buttonControlStates buttonState;
static uint32_t pressTime;      ///< Time button was pressed.
static bool buttonHeldActive;   ///< Flag that determines if we already sent a message to the LED task in the current <1 second press.

/******************************************************************************
 * Forward Declarations
 ******************************************************************************/

/******************************************************************************
 * Callback Functions
 ******************************************************************************/

/**************************************************************************/ /**
 * @fn		void ButtonTask(void)
 * @brief	Initializes the button task. Only run once at the start! Assumes button has already been initialized.

 * @param[in]
 * @param[out]
 * @return
 * @note
 *****************************************************************************/
void InitButtonTask(void) {
	
	struct port_config config_port_pin;
    port_get_config_defaults(&config_port_pin);

	// button
    config_port_pin.direction  = PORT_PIN_DIR_INPUT;
    config_port_pin.input_pull = PORT_PIN_PULL_UP;
    port_pin_set_config(BUTTON_0_PIN, &config_port_pin);

	//led
    config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(LED_0_PIN, &config_port_pin);
	
	buttonState = BUTTON_RELEASED;
	pressTime = 0;
	buttonHeldActive = false;
	
}

/**************************************************************************/ /**
 * @fn		void LedTask(void)
 * @brief	Runs the LED TASK

 * @param[in]
 * @param[out]
 * @return
 * @note
 *****************************************************************************/
void ButtonTask(void *pvParameters) {
	//const TickType_t xDelay = pdMS_TO_TICKS(10);
	//const unsigned long ulValueToSend = 100UL;
	//SerialConsoleWriteString("ButtonStart.\n");

	InitButtonTask();
	for (;;)
	{
		//press
		if(buttonState == BUTTON_RELEASED && !port_pin_get_input_level(BUTTON_0_PIN)){
			buttonState = BUTTON_PRESSED;
			SerialConsoleWriteString("1");
			port_pin_set_output_level(LED_0_PIN, 0);
		}
		//release
		if(buttonState == BUTTON_PRESSED && port_pin_get_input_level(BUTTON_0_PIN)){
			buttonState = BUTTON_RELEASED; // Reset button state
			port_pin_set_output_level(LED_0_PIN, 1);
			SerialConsoleWriteString("2");
		}
	}
}