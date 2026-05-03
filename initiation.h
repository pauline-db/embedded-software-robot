/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef INITIATION_H
#define INITIATION_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
int pico_led_init(void);
void encoder_init(void);
void motor_pwm_init(uint pin);
void motor_pwm_init_all(void);
void hcsr04_pio_init(int pin_trigger, int pin_echo, PIO pio, uint sm);
void tcrt5000_init(void);

#endif