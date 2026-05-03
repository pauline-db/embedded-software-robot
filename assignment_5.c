/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/

#include "assignment_5.h"
#include "definitions_file.h"
#include "motor_functions.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
// -------------------- CONFIG (EDIT THESE) --------------------

// Encoder GPIOs (you said each wheel has ONE encoder signal)
#ifndef ENC_LEFT
#define ENC_LEFT   ENC1_1   // rename your macros in definitions_file.h if you can
#endif
#ifndef ENC_RIGHT
#define ENC_RIGHT  ENC1_2
#endif

// Wheel geometry
#define WHEEL_RADIUS_M        (0.0325f)   // you used 0.03 m
#define TARGET_DISTANCE_M     (1.6f)

// Encoder resolution (pulses per wheel revolution)
// IMPORTANT: set this to the real value for your encoder
#define PULSES_PER_REV        (65.0f)  //adapted for encoder (even though 20 holes)

// Safety
#define CONTROL_DT_MS         (20)      // loop period for control
#define STARTUP_TIMEOUT_MS    (2000)    // wait max 2s for ticks to start
#define MAX_RUN_TIME_MS       (15000)   // hard cap 15s so it never runs forever

// -------------------- GLOBALS --------------------

static volatile uint32_t left_ticks  = 0;
static volatile uint32_t right_ticks = 0;

// -------------------- IRQ HANDLER --------------------

void encoder_irq_handler(uint gpio, uint32_t events) {
    (void)events;
    if (gpio == ENC_LEFT)  left_ticks++;
    if (gpio == ENC_RIGHT) right_ticks++;
}

// -------------------- ENCODER INIT --------------------

static void encoder_setup_once(void) {
    static bool inited = false;
    if (inited) return;

    // Configure pins as inputs
    gpio_init(ENC_LEFT);
    gpio_set_dir(ENC_LEFT, GPIO_IN);
    gpio_pull_up(ENC_LEFT);   // often required for hall/open-collector encoders

    gpio_init(ENC_RIGHT);
    gpio_set_dir(ENC_RIGHT, GPIO_IN);
    gpio_pull_up(ENC_RIGHT);

    // Install callback on first pin, then enable IRQ on both pins
    gpio_set_irq_enabled_with_callback(
        ENC_LEFT,
        GPIO_IRQ_EDGE_RISE,   // use RISE|FALL if you want double resolution
        true,
        &encoder_irq_handler
    );
    gpio_set_irq_enabled(
        ENC_RIGHT,
        GPIO_IRQ_EDGE_RISE,
        true
    );

    inited = true;
}



static inline void stop_motors(void) {
    set_motor_speed(A1A, 0); set_motor_speed(A1B, 0);
    set_motor_speed(B1A, 0); set_motor_speed(B1B, 0);
}

static inline uint32_t ticks_avg(void) {
    // average distance traveled by two wheels (helps if one slips)
   return (left_ticks + right_ticks) / 2u;

}

static inline float distance_per_tick_m(void) {
    // wheel circumference / pulses per revolution
    const float circ = 2.0f * 3.14159265f * WHEEL_RADIUS_M;
    return circ / PULSES_PER_REV;
}

// -------------------- MAIN ASSIGNMENT FUNCTION --------------------

void assignment_5_function(int* speed, mode_assignment* active_assignment) {
    encoder_setup_once();

    // Reset counters for this run
    left_ticks = 0;
    right_ticks = 0;

    // Compute target ticks
    const float dpt = distance_per_tick_m();
    const uint32_t target_ticks = (uint32_t)(TARGET_DISTANCE_M / dpt);

    // Make sure we actually start moving
    absolute_time_t kick = get_absolute_time();
    while (absolute_time_diff_us(kick, get_absolute_time()) < 300000) {
        *speed = 80;
        line_follower_function(speed);
        sleep_ms(20);
    }

        // Drive until target distance reached or safety timeout
        absolute_time_t t0 = get_absolute_time();
        while (ticks_avg() < target_ticks) {
            printf("Left ticks: %lu, Right ticks: %lu, Avg: %lu\n", left_ticks, right_ticks, ticks_avg());
            *speed = 80;
            line_follower_function(speed);

           
            sleep_ms(CONTROL_DT_MS);
        }

    // Stop

    set_motor_speed(A1A, 0); set_motor_speed(A1B, 0);
    set_motor_speed(B1A, 0); set_motor_speed(B1B, 0);
    sleep_ms(10000); // brief pause

    // IMPORTANT: leave assignmen
}