/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#ifndef ASSIGNMENT_5_H
#define ASSIGNMENT_5_H
#include "pico/stdlib.h"    
#include "definitions_file.h"
void assignment_5_function(int* speed, mode_assignment* active_assignment);
float get_motor_rpm(void);
void encoder_irq_handler(uint gpio, uint32_t events);
#endif