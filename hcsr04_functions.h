/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef HCSR04_FUNCTIONS_H
#define HCSR04_FUNCTIONS_H
#include "pico/stdlib.h"    
#include "definitions_file.h"
#include "hardware/pio.h"
uint32_t hcsr04_read_echo_us(int pin_trigger, int pin_echo, PIO pio, uint sm);

#endif