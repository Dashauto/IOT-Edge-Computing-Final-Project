/*
 * LedTask.c
 *
 * Created: 2024/4/15 18:18:20
 *  Author: YINING
 */ 
#include "FreeRTOS.h"
#include "task.h"
#include "LedTask.h"
#include "asf.h"
#include "SerialConsole.h"

#include "queue.h"
#include "Global.h"


/**************************************************************************/ /**
 * @fn		void ButtonTask(void)
 * @brief	Initializes the button task. Only run once at the start! Assumes button has already been initialized.

 * @param[in]
 * @param[out]
 * @return
 * @note
 *****************************************************************************/
void InitLedTask(void) {
    
	struct port_config config_port_pin;
    port_get_config_defaults(&config_port_pin);
    config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(LED_0_PIN, &config_port_pin);
	
}

/**************************************************************************/ /**
 * @fn		void LedTask(void)
 * @brief	Runs the LED TASK

 * @param[in]
 * @param[out]
 * @return
 * @note
 *****************************************************************************/
void LedTask(void *pvParameters) {
	//const TickType_t xDelay = pdMS_TO_TICKS(10);
	//const unsigned long ulValueToSend = 100UL;
	//SerialConsoleWriteString("ButtonStart.\n");
    InitLedTask();
	for (;;)
	{
		port_pin_toggle_output_level(LED_0_PIN);
        vTaskDelay(1000);
	}
}