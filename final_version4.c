#include "assignment_4.h"
#include "definitions_file.h"
#include "motor_functions.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdbool.h>
#include "hcsr04_functions.h"
#include "initiation.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "if_echo.pio.h"
#include <stdio.h>

void avoid_object_function(int* speed, bool* obj_cleared);
void line_follower_function(int* speed);
void minima_counter_fct(int* speed, int* previous_dist, bool* turn_finished);

static PIO pio_1 = pio0; 
// rasberry pi pico has two pio blocks: pio0 and pio1
// we use pio0 here (each block has 4 state machines)
static uint sm1 = 0; // state machine index
static PIO pio2 = pio1; 
// rasberry pi pico has two pio blocks: pio0 and pio1
// we use pio0 here (each block has 4 state machines)
static uint sm2 = 0; // state machine index



void turn(int* speed){
    *speed = 80;
    set_motor_speed(A1A, 0);
    set_motor_speed(A1B, 0);
    set_motor_speed(B1A,  *speed);
    set_motor_speed(B1B,  0);

    return;
}

// turn left (in-place) helper (minimal addition)
static void turn_left(int* speed){
    *speed = 80;
    set_motor_speed(A1A, *speed);
    set_motor_speed(A1B, 0);
    set_motor_speed(B1A, 0);
    set_motor_speed(B1B, 0);
    return;
}


void avoid_object_function(int* speed, bool* obj_cleared){
    static bool initialized_2 = false;
    if (!initialized_2) {
        hcsr04_pio_init(SRF2_T, SRF2_E, pio2, sm2);
        initialized_2 = true;
    }

    *speed = 80;

    // --- Start turning left until we reach the minimum distance (initial alignment) ---
    bool turn_finished = false;
    int previous_dist = 10000;

    absolute_time_t turn_start = get_absolute_time();
    /*do {
        minima_counter_fct(speed, &previous_dist, &turn_finished);
        
        // timeout: don't get stuck turning forever
        if (absolute_time_diff_us(turn_start, get_absolute_time()) > 5 * 1000 * 1000) {
            turn_finished = true; // bail out
        }
    } while (!turn_finished);*/
    *speed = 80;
    turn_left(speed);
    sleep_ms(1000);
    // start "initial phase" timer (to avoid turning right due to echo==0 just after the initial left turn)
    absolute_time_t initial_left_phase_start = get_absolute_time();

    // --- Now keep avoiding until we detect the black line again ---
    absolute_time_t avoid_start = get_absolute_time();

    // WAIT 10 seconds before looking for the black line so he can pass trough the first turn with no problem
    absolute_time_t ir_delay_start = get_absolute_time();

    while (true) {

        uint32_t echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);

        // --- IR line detection ---
        // (after 10s only)
        if (absolute_time_diff_us(ir_delay_start, get_absolute_time()) > 10 * 1000 * 1000) {
            adc_select_channel(0); // select IR1_3
            int left_sensor_value = adc_read_register();
            adc_select_channel(1); // select IR2_3
            int right_sensor_value = adc_read_register();

            bool left_sensor  = (left_sensor_value  < 1000) ? 0 : 1;
            bool right_sensor = (right_sensor_value < 1000) ? 0 : 1;

            if (left_sensor || right_sensor) {
                *obj_cleared = true;
                turn_left(speed); // small left to better align with line
                sleep_ms(1100);
                return;
            }
        }

        //if distance greater than 20 turn if smaller go forward
        //if sensor detects 0 then it should also turn as this means it does not detect anything
        //the goal is also to average the values to decrease effect of noise

        // handle echo_us2==0 with a limited number of retries + small spacing
        if (echo_us2 == 0) {
            int zeros = 0;
            for (int k = 0; k < 4; k++) {
                sleep_ms(20);
                uint32_t v0 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
                if (v0 == 0) zeros++;
            }

            if (zeros >= 4) {
                // treat as really zero
                // during the first 2 seconds after the initial turn, keep turning LEFT
                if (absolute_time_diff_us(initial_left_phase_start, get_absolute_time()) < 4 * 1000 * 1000) {
                    turn_left(speed);
                } else {
                    turn(speed); // your normal behavior when 0
                }
                sleep_ms(30);
                continue;
            } else {
                // not really zero -> take one more reading
                echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
                if (echo_us2 == 0) {
                    // if still 0, apply the same rule as above
                    if (absolute_time_diff_us(initial_left_phase_start, get_absolute_time()) < 2 * 1000 * 1000) {
                        turn_left(speed);
                    } else {
                        turn(speed);
                    }
                    sleep_ms(30);
                    continue;
                }
            }
        }


        {
            uint32_t sum = 0;
            uint32_t count = 0;

            // include current non-zero
            if (echo_us2 != 0) {
                sum += echo_us2;
                count++;
            }

            // try to add up to 3 more non-zero samples
            int tries = 0;
            while (count < 4 && tries < 6) { // allow some retries to skip zeros
                tries++;
                sleep_ms(20);
                uint32_t v = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
                if (v != 0) {
                    sum += v;
                    count++;
                }
            }

            if (count > 0) {
                echo_us2 = sum / count;
            } else {
                // if we somehow got only zeros here, treat as zero
                if (absolute_time_diff_us(initial_left_phase_start, get_absolute_time()) < 2 * 1000 * 1000) {
                    turn_left(speed);
                } else {
                    turn(speed);
                }
                sleep_ms(30);
                continue;
            }
        }

        uint32_t distance_cm = echo_us2 / 58u;

        // Added detail:
        // if its closer than 15 cm then it turns left
        // if its farther than 25 it turns right
        // otherwise go forward (between 15 and 25 cm)
        if (distance_cm < 15u) {
            turn_left(speed);
        } else if (distance_cm > 20u || distance_cm == 0u) {
            uint32_t echo_us=hcsr04_read_echo_us(SRF1_T, SRF1_E, pio1, sm1);
            if(echo_us==0 || (echo_us / 58u)>50u){
                // go forward
                turn(speed);
                
            } 
            else if((echo_us / 58u)<20u){
                turn_left(speed);
            } 
            
        } else {
            // go forward
            *speed = 80;
            set_motor_speed(A1A, *speed);
            set_motor_speed(A1B, 0);
            set_motor_speed(B1A, *speed);
            set_motor_speed(B1B, 0);
        }

        // small delay helps sensor stability + avoids hammering reads
        sleep_ms(30);
    }
}



void minima_counter_fct(int* speed, int* previous_dist, bool* turn_finished){
    absolute_time_t t0 = get_absolute_time();

    uint32_t echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);

    // If sensor reads 0, keep turning left BUT with a timeout
    while (echo_us2 == 0 || echo_us2 / 58u > 100u) { // 100 cm
        *speed=80;
        turn_left(speed);

        // timeout: don't spin forever if sensor fails
        if (absolute_time_diff_us(t0, get_absolute_time()) > 50*1000 * 1000) { // 500ms
            *turn_finished = true; // bail out
            printf("Distance1 timeout: echo stayed 0\n");
            return;
        }
    
        
        echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
    }

    // average a few readings to reduce noise (ignore zeros)
    {   
        uint32_t zeros = 0;
        while (echo_us2 == 0) {
            if (zeros > 4) {
                if (absolute_time_diff_us(t0, get_absolute_time()) > 2* 1000 * 1000) { // 500ms
                    *turn_finished = true;
                    break;
                }
                
            }
            zeros++;
            echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
        }
        uint32_t sum = 0;
        uint32_t i = 0;
        while (i<3){
            if (echo_us2 != 0) {
                sum += echo_us2;
                i++;
            }
            echo_us2 = hcsr04_read_echo_us(SRF2_T, SRF2_E, pio2, sm2);
        }


        if (i > 0) {
            echo_us2 = sum / i;
        } else {
            if (absolute_time_diff_us(t0, get_absolute_time()) > 1*1000 * 1000) {// 2 {sec}
                *turn_finished = true;
                return;
            }
        }
    }

    uint32_t distance_cm = echo_us2 / 58u;
    printf("Distance1 cm: %lu\n", (unsigned long)distance_cm);
    bool first_value = true;
    if (distance_cm < 100u && distance_cm != 0u){//so that when it sees in the distance more than 100 cm it doesnt consider it
        if (distance_cm < (uint32_t)(*previous_dist)+2u) {
            
            *previous_dist = (int)distance_cm;

            // keep turning left
            *speed = 80;
            set_motor_speed(A1A, *speed);
            set_motor_speed(A1B, 0);
            set_motor_speed(B1A, 0);
            set_motor_speed(B1B, 0);
            return;
            
        } else {
            // minimum reached -> go forward
            if (absolute_time_diff_us(t0, get_absolute_time()) > 1*1000 * 1000) { // 500ms
                *turn_finished = true;
            }

            
            return;
        }
    }
}    







//echo and trigger pins for side sensor : SRF2_E and SRF2_T
void assignment_4_function(int* speed, mode_assignment* active_assignment){

    pico_set_led(true);
    // Obstacle avoidance logic
    //continue to  follow line unless obstacle detected within 20 cm
    //Use side sensor to know when obstacle has been avoided
    static bool obj_detected = false;
    static bool obj_cleared = false;    
    static bool initialized_1 = false;
    if (!initialized_1 ) {
        hcsr04_pio_init(SRF1_T, SRF1_E, pio_1, sm1);//front sensor
        initialized_1 = true;
    }

    uint32_t echo_us1 = hcsr04_read_echo_us(SRF1_T, SRF1_E, pio_1, sm1);//get echo time

    if (echo_us1 == 0) { // if never triggers/received full speed
        //no obstacle
        *speed = 80;
        line_follower_function(speed);

    } else { //distance in cm is echo time divided by 58
        uint32_t distance_cm = echo_us1 / 58u;
        if (distance_cm < 20u) {

            obj_detected = true;
            obj_cleared = false;

            set_motor_speed(A1A, 0);//for forward 1,0,1,0, backwards 0,1,0,1 (with 1 the speed)
            set_motor_speed(A1B, 0);
            set_motor_speed(B1A, 0);
            set_motor_speed(B1B, 0);
        } else {
            *speed = 80;
            line_follower_function(speed);
        }
    }

    if (obj_detected && !obj_cleared) {
        //avoid object mode

        avoid_object_function(speed, &obj_cleared);
        // check front sensor if not clear turn left else check side sensor
        //if side sensor clear turn right else go forward
        //and check side sensor again
        //once again check side sensor if clear turn right else go forward and check again
        obj_detected = false;
        obj_cleared = false;
        //check ir continuoulsy untill black line detected and then go back to line follower mode and set object detected to false
    } 

    return;
}
