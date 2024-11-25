#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/adc.h"

#include "main.h"

#include <DmxOutput.h>

DmxOutput dmx;

uint8_t universe[UNIVERSE_LENGTH + 1];


int main () {

    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);


    dmx.begin(0);
    universe[1] = 0;
    universe[2] = 255;
    universe[3] = 0;
    universe[4] = 0;
    universe[5] = 255;


while (1) {
    dmx.write(universe, UNIVERSE_LENGTH);
    while (dmx.busy()) {}

    adc_select_input(0);
    universe[1] = adc_read() / 16;
    adc_select_input(1);
    universe[3] = adc_read() / 16;

}
};