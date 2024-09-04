#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"

#include "hardware/adc.h"

#include <DmxOutput.h>

DmxOutput dmx;

#define UNIVERSE_LENGTH 512
uint8_t universe[UNIVERSE_LENGTH + 1];


int main () {

    stdio_init_all();
    if (cyw43_arch_init()) {
        return -1;
    };

    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);


    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

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