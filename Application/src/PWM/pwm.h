/*
 * pwm.c
 *
 * Created: 2024/4/18 20:18:52
 *  Author: wz, yx
 */ 
 #include <asf.h>


#ifndef PWM_H_
#define PWM_H_

#define PWM_MODULE EXT1_PWM_MODULE
#define PWM_OUT_PIN EXT1_PWM_0_PIN
#define PWM_OUT_MUX EXT1_PWM_0_MUX

// f = 48Mhz / (2 * prescaler * (1 + PER))
// prescaler = 1, PER = 0xA1A3, f = 580Hz
// prescaler = 1, PER = 0x7FFF, f = 732Hz
#define PWM_OPEN_frequncy 0xA1A3
#define PWM_CLOSE_frequncy 0x7FFF

// #define EXT1_PWM_MODULE           TCC0
// #define EXT1_PWM_0_CHANNEL        2
// #define EXT1_PWM_0_PIN            PIN_PA10F_TCC0_WO2
// #define EXT1_PWM_0_MUX            MUX_PA10F_TCC0_WO2


void pwm_func_confg(void);
void start_pwm(void);
void stop_pwm(void);


#endif /* PWM_H_ */