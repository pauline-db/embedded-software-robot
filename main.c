/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "definitions_file.h"
#include "hardware/pwm.h"
#include "initiation.h"
#include "motor_functions.h"
#include "assignment_1.h"
#include "assignment_2.h"
#include "assignment_3.h"
#include "assignment_4.h"
#include "assignment_5.h"
// This is needed to send the Arduino servo-specific commands
// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

static mode_assignment active_assignment = ASSIGNMENT_5;

static void stop_motors() {
    set_motor_speed(A1A, 0);
    set_motor_speed(A1B, 0);
    set_motor_speed(B1A, 0);
    set_motor_speed(B1B, 0);
    sleep_ms(10000);
}

int main() {
    stdio_init_all();
    encoder_init();
    motor_pwm_init_all();
    int rc = pico_led_init();

    hard_assert(rc == PICO_OK);
    motor_pwm_init_all();
    absolute_time_t start_time = get_absolute_time();   
    int speed = 0;
    while(ASSIGNMENT_1 ==  active_assignment) {
        assignment_1_function(start_time, &speed , &active_assignment);
    }
    while (ASSIGNMENT_2 == active_assignment) {
        assignment_2_function( &speed, &active_assignment);
        
    }
    absolute_time_t t1 = get_absolute_time();
    while (ASSIGNMENT_3 == active_assignment) {
        assignment_3_function( &speed, &active_assignment);
        
        /*if (absolute_time_diff_us(t1, get_absolute_time()) > 30000000) {
            active_assignment = ASSIGNMENT_4;
            stop_motors();
            
        }*/
    }
    absolute_time_t t2 = get_absolute_time();
    while (ASSIGNMENT_4 == active_assignment) {
        
        assignment_4_function( &speed, &active_assignment);
        /*if (absolute_time_diff_us(t2, get_absolute_time()) > 60000000) {
            // after 60 secondsswitch to assignment 5
            active_assignment = ASSIGNMENT_5;
            stop_motors();
            
        } */
    }

    while (ASSIGNMENT_5 == active_assignment) {
        
        assignment_5_function( &speed, &active_assignment);
    }
}
