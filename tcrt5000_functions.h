/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef TCRT5000_FUNCTIONS_H
#define TCRT5000_FUNCTIONS_H
#include "pico/stdlib.h"    
#include "definitions_file.h"
#include "hardware/pio.h"
void adc_register_init();
void adc_select_channel(int channel);
uint16_t adc_read_register();

#endif
