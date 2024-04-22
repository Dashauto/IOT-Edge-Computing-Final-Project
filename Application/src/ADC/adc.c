/*
 * adc.c
 *
 * Created: 2024/4/18 16:13:02
 *  Author: yn
 */ 

#include "adc.h"
#include "SerialConsole.h"

struct adc_module adc_instance;

#define ADC_SAMPLES 128
uint16_t adc_result_buffer[ADC_SAMPLES];

volatile bool adc_read_done = false;
void adc_complete_callback(const struct adc_module *conmodule)
{
    adc_read_done = true;
}

    
void configure_adc(void)
{
    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);
    config_adc.gain_factor = ADC_GAIN_FACTOR_DIV2;
    config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
    config_adc.reference = ADC_REFERENCE_INTVCC1;
    config_adc.positive_input = ADC_POSITIVE_INPUT_PIN0;  // PA02 -- PIN0
    config_adc.resolution = ADC_RESOLUTION_12BIT;
    adc_init(&adc_instance, ADC, &config_adc);
    adc_enable(&adc_instance);
}

void configure_adc_callbacks(void)
{
    adc_register_callback(&adc_instance, adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
    adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
}

/*initialize*/
void adc_func_config(void){
    configure_adc();
    configure_adc_callbacks();
}


int adc_read_raw(void) {
    system_interrupt_enable_global();
    adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);
    
    int timeout = 10000; // Set timeout limit for the while loop (this value may need to be adjusted based on the expected response time)
    int counter = 0;

    while (!adc_read_done) {
        if (counter++ > timeout) {
            SerialConsoleWriteString("\r\nADC read timeout\r\n");
            return -1; // Return a specific error code for timeout
        }
    }

    // reset the flag
    adc_read_done = false;

    uint32_t sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += adc_result_buffer[i];
    }
    return (uint16_t)(sum / ADC_SAMPLES);

}
