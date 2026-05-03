/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "assignment_3.h"
#include "definitions_file.h"
#include "motor_functions.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdbool.h>
#include "initiation.h"



void assignment_3_function(int* speed, mode_assignment* active_assignment) {
    // Line follower functionality
    // two TCRT5000 sensors connected to IR1_3 IR1_4 and IR2_3 IR2_4
    //IR1_4 is the input from left sensor
    //IR2_4 is the input from right sensor  
    //when high IR1_3 and IR2_3 are pulled high (5V) through a resistor
    //when the sensor detects black surface the output goes low (0V)
   line_follower_function(speed);
}