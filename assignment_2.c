/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "assignment_2.h"
#include "definitions_file.h"
#include "motor_functions.h"
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "initiation.h"
#include "hcsr04_functions.h"



static PIO pio = pio0; 
// rasberry pi pico has two pio blocks: pio0 and pio1
// we use pio0 here (each block has 4 state machines)
static uint sm = 0; // state machine index





void assignment_2_function(int* speed, mode_assignment* active_assignment) {
    //10 us high level signal using IO trigger
    static bool inited = false;
    if (!inited) {
        hcsr04_pio_init(SRF1_T, SRF1_E, pio, sm);//front sensor
        inited = true;
    }

    uint32_t echo_us = hcsr04_read_echo_us(SRF1_T, SRF1_E, pio, sm);//get echo time
    if (echo_us == 0) { // if never triggeres/received full speed
        //no obstacle
        *speed = 100;
    } else { //distance in cm is echo time divided by 58
        uint32_t distance_cm = echo_us / 58u;
        *speed = (distance_cm < 15u) ? 0 : 100;
    }
    set_motor_speed(A1A, *speed);//for forward 1,0,1,0, backwards 0,1,0,1 (with 1 the speed)
    set_motor_speed(A1B, 0);

    set_motor_speed(B1A, *speed);
    set_motor_speed(B1B, 0);
    if (*speed == 0) {
        *active_assignment = ASSIGNMENT_3;
        
        set_motor_speed(A1A, 0);
        set_motor_speed(A1B, 0);
        set_motor_speed(B1A, 0);
        set_motor_speed(B1B, 0);
        sleep_ms(10000);

    }
    //module sends eight 40 kHz pulses
    // IF signal back test distance =(high level time x 340 M/s)/2

}