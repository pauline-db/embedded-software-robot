/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "pico/stdlib.h"
#include "motor_functions.h"
#include "hardware/pwm.h"
#include "definitions_file.h"
#include "initiation.h"
#include "tcrt5000_functions.h"
#include <stdio.h>




//--------------MOTOR FUNCTIONS----------------//
void set_motor_speed(int motor_pin, int speed){// speed from 0 to 100(wrap value)
    pwm_set_gpio_level(motor_pin, speed);
}


//--------------TEMPORARY CHECK FUNCTIONS----------------//
void pico_set_led(bool led_on) {//TEMPORARY
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void line_follower_function(int* speed) {
    // Line follower functionality
    // two TCRT5000 sensors connected to IR1_3 IR1_4 and IR2_3 IR2_4
    //IR1_4 is the input from left sensor
    //IR2_4 is the input from right sensor  
    //when high IR1_3 and IR2_3 are pulled high (5V) through a resistor
    //when the sensor detects black surface the output goes low (0V)
    static bool initialized_IR = false;
    if (!initialized_IR) {
        adc_register_init();
        tcrt5000_init();
        initialized_IR = true;
    }
    
    
    

    //wait for sensor to stabilize
    adc_select_channel(0); //select IR1_3
    int left_sensor_value = adc_read_register(); //read left sensor
    //printf("Left sensor value: %d\n", left_sensor_value);
    adc_select_channel(1); //select IR2_3
    int right_sensor_value = adc_read_register(); //read right sensor
    //printf("Right sensor value: %d\n", right_sensor_value);
    bool left_sensor = (left_sensor_value < 1000)? 0 : 1; //read left sensor
    bool right_sensor = (right_sensor_value < 1000)? 0 : 1; //read right sensor
    if (left_sensor && !right_sensor) {
        // left sensor on black surface, turn left

        *speed = 80;
        set_motor_speed(A1A, 0);
        set_motor_speed(A1B, *speed);
        set_motor_speed(B1A, *speed);
        set_motor_speed(B1B, 0);
    } 
    else if (!left_sensor && right_sensor) {
        // right sensor on black surface, turn right

        *speed = 80;
        set_motor_speed(A1A, *speed);
        set_motor_speed(A1B, 0);
        set_motor_speed(B1A, 0);
        set_motor_speed(B1B, *speed);
    } 
    else if(!left_sensor && !right_sensor){
        // both sensors on white surface, move forward

        *speed = 80;
        set_motor_speed(A1A, *speed);
        set_motor_speed(A1B, 0);
        set_motor_speed(B1A, *speed);
        set_motor_speed(B1B, 0);

    }

}
