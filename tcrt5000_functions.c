/* *
* Student name : [ PAULINE DE BAETS ]
* Student number : [ 6544819 ]
*
*/
#include "tcrt5000_functions.h"
#include "hardware/regs/adc.h"
#include "hardware/regs/resets.h"
#include "hardware/structs/adc.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "initiation.h"
#include "definitions_file.h"


void adc_register_init() {
    // Take ADC out of reset
    reset_block(RESETS_RESET_ADC_BITS);
    unreset_block_wait(RESETS_RESET_ADC_BITS);

    // Enable ADC
    adc_hw->cs = ADC_CS_EN_BITS;
}

void adc_select_channel(int channel) {
    adc_hw->cs = (adc_hw->cs & ~ADC_CS_AINSEL_BITS) | (channel << ADC_CS_AINSEL_LSB);
}

uint16_t adc_read_register() {
    adc_hw->cs |= ADC_CS_START_ONCE_BITS;      // Start conversion
    while (!(adc_hw->cs & ADC_CS_READY_BITS)); // Wait until done
    return adc_hw->result;                     // 12‑bit result (0–4095)
}