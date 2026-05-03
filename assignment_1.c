/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "pico/stdlib.h"      // absolute_time_t, get_absolute_time, sleep_ms, etc.
#include "hardware/pwm.h"     // pwm_set_gpio_level (used by set_motor_speed)
#include "definitions_file.h" // A1A/A1B/B1A/B1B, assignment_t, active_assignment declaration
#include "motor_functions.h"            // declaration of set_motor_speed (or wherever you put it)
#include "assignment_1.h"       // declaration of assignment_1_functions (your header)



void assignment_1_function(absolute_time_t start_time,  int* speed, mode_assignment* active_assignment) {  
        pico_set_led(true);
        int64_t ms = absolute_time_diff_us(start_time, get_absolute_time()) / 1000;
        if (ms < 4000){
            *speed = 80;
        }
        else if (ms > 4000 && ms < 8000){
            *speed = 100;
        }
        else if (ms > 8000 && ms < 12000 && *speed > 0){
            int64_t t = ms  - 8000;              
            *speed = 100 - (int)((t * 100) / 4000); 
            if (*speed < 0) *speed = 0;
        }
        else if (ms > 12000){
            *active_assignment = ASSIGNMENT_2;
        }
        set_motor_speed(A1A, *speed);//for forward 1,0,1,0, backwards 0,1,0,1 (with 1 the speed)
        set_motor_speed(A1B,0);

        set_motor_speed(B1A, *speed);
        set_motor_speed(B1B, 0);
    }
    