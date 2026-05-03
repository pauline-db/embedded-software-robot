/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef MOTOR_FUNCTIONS_H
#define MOTOR_FUNCTIONS_H
#include "hardware/regs/adc.h"
#include "hardware/regs/resets.h"
#include "hardware/structs/adc.h"
void set_motor_speed(int motor_pin, int speed);
void pico_set_led(bool led_on);
void line_follower_function(int* speed);
void acd_register_init(void);
void adc_select_channel(int channel);
uint16_t adc_read_register() ;


#endif