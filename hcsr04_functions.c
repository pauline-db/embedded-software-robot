
/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "hcsr04_functions.h"
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





uint32_t hcsr04_read_echo_us(int pin_trigger, int pin_echo, PIO pio, uint sm) { //do one distance measurement
   
    // Reset SM + FIFOs and run
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    pio_sm_set_enabled(pio, sm, true);
     // Trigger pulse: 10 us HIGH
    gpio_put(pin_trigger, 0);
    sleep_us(2);
    gpio_put(pin_trigger, 1);// send trigger pulse for 10 us
    sleep_us(10);
    gpio_put(pin_trigger, 0);
    // Blocking read: gets X_final from FIFO
    absolute_time_t t0 = get_absolute_time();
    while (pio_sm_is_rx_fifo_empty(pio, sm)) {
        if (absolute_time_diff_us(t0, get_absolute_time()) > 30000) {
            pio_sm_set_enabled(pio, sm, false);
            return 0; // timeout -> no echo received
        }
        tight_loop_contents();
    }
    uint32_t x_final = pio_sm_get(pio, sm);

    // elapsed_ticks = 0xFFFFFFFF - x_final
    // With our 2 MHz PIO clock + 2-instruction decrement loop => ticks == microseconds
    uint32_t echo_us = 0xFFFFFFFFu - x_final; // since we decrement because of the way the jmp works in pio
    
    pio_sm_set_enabled(pio, sm, false);
    return echo_us;
}
