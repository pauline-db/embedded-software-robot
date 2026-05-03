/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "definitions_file.h"
#include "initiation.h"
#include <stdbool.h>
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "initiation.h"
#include "if_echo.pio.h"
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif



//--------------LED INITIALIZATION FUNCTION----------------//
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    return cyw43_arch_init();
#endif
}

//--------------MOTOR CONTROL FUNCTIONS----------------//

void encoder_init(){
    gpio_init(ENC1_1);
    gpio_set_dir(ENC1_1, GPIO_IN);
    gpio_init(ENC1_2);
    gpio_set_dir(ENC1_2, GPIO_IN);
    //attach interrups

}

//--------------SPEED CONTROL FUNCTIONS----------------//
void motor_pwm_init(uint pin){
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);

    pwm_set_wrap(slice_num, 100);        // speed scale 0..100
    pwm_set_clkdiv(slice_num, 62.0f);    // ~20kHz
    pwm_set_enabled(slice_num, true);
}


void motor_pwm_init_all(void) {
    motor_pwm_init(A1A);
    motor_pwm_init(A1B);
    motor_pwm_init(B1A);
    motor_pwm_init(B1B);

    // start stopped
    pwm_set_gpio_level(A1A, 0);
    pwm_set_gpio_level(A1B, 0);
    pwm_set_gpio_level(B1A, 0);
    pwm_set_gpio_level(B1B, 0);
}
//--------------HC-SR04 ULTRASONIC SENSOR FUNCTIONS----------------//


void hcsr04_pio_init(int pin_trigger, int pin_echo, PIO pio, uint sm) {
    // Store program offset once per PIO block
    static int offset_pio0 = -1;
    static int offset_pio1 = -1;

    int offset;
    if (pio == pio0) {
        if (offset_pio0 < 0) {
            offset_pio0 = pio_add_program(pio0, &if_echo_program);
        }
        offset = offset_pio0;
    } else { // pio1
        if (offset_pio1 < 0) {
            offset_pio1 = pio_add_program(pio1, &if_echo_program);
        }
        offset = offset_pio1;
    }
    
    // TRIG pin: normal GPIO output
    gpio_init(pin_trigger);
    gpio_set_dir(pin_trigger, GPIO_OUT);
    gpio_put(pin_trigger, 0);

    // ECHO pin: PIO input
    pio_gpio_init(pio, pin_echo);
    gpio_pull_down(pin_echo); // helps if echo line floats
    pio_sm_set_consecutive_pindirs(pio, sm, pin_echo, 1, false); // echo is input

    // Build SM config from program defaults
    pio_sm_config c = if_echo_program_get_default_config(offset);

    // CRITICAL: map "pin 0" and "jmp pin" to the ECHO GPIO
    sm_config_set_in_pins(&c, pin_echo);
    sm_config_set_jmp_pin(&c, pin_echo);

    // Set PIO clock so that your echo_us math makes sense
    // (You said you target 2 MHz in your comments)
    float div = (float)clock_get_hz(clk_sys) / 2000000.0f;
    sm_config_set_clkdiv(&c, div);

    // Init + keep SM disabled until a read
    pio_sm_init(pio, sm, (uint)offset, &c);
    pio_sm_set_enabled(pio, sm, false);
}



void tcrt5000_init(void) {
    //left sensor
    gpio_init(IR1_4);
    gpio_set_dir(IR1_4, GPIO_IN);

    //right sensor
    gpio_init(IR2_4);
    gpio_set_dir(IR2_4, GPIO_IN);

}