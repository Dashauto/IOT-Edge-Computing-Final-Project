/*
 * pwm.c
 *
 * Created: 2024/4/18 20:18:02
 *  Author: wz, yx
 */ 

#include "pwm.h"

struct tcc_module tcc_instance;
struct tcc_config config_tcc;

void configure_tc(void)
{
    tcc_get_config_defaults(&config_tcc, TCC1);
    config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV1; // Adjust as needed
    config_tcc.counter.period = (0xFFFF / 2); // Implement this function based on your needs
    //config_tcc.counter.period = 0x14ED2;
    config_tcc.pins.enable_wave_out_pin[2] = true;
    config_tcc.pins.wave_out_pin[2] = PIN_PA10E_TCC1_WO0;
    config_tcc.pins.wave_out_pin_mux[2] = PINMUX_PA10E_TCC1_WO0;

    tcc_init(&tcc_instance, TCC1, &config_tcc);
}


void pwm_func_confg(void)
{
    configure_tc();
}

void start_pwm(int forward) 
{
    if (forward) 
    {
        config_tcc.counter.period = PWM_OPEN_frequncy;
    } 
    else
    {
        config_tcc.counter.period = PWM_CLOSE_frequncy;
    }
    tcc_init(&tcc_instance, TCC1, &config_tcc);
    tcc_enable(&tcc_instance);
}

void stop_pwm(void) 
{
    tcc_disable(&tcc_instance);
}
