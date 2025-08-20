
#include <stdio.h>

#include "ssd1306.h"
#include "hardware/gpio.h"

int main() {
    stdio_usb_init();
    stdio_init_all();

    sleep_ms(1000);

    i2c_init(i2c0, 100 * 1000);

    gpio_init(8);
    gpio_init(9);

    gpio_set_function(8, GPIO_FUNC_I2C);
    gpio_set_function(9, GPIO_FUNC_I2C);

    gpio_pull_up(8);
    gpio_pull_up(9);

    ssd1306_display_t Display;

    ssd1306_init(&Display, i2c0, 0x3c, 128, 64);
    ssd1306_draw_line(&Display, 0, 0, 128, 64, ADD);
    printf("line");
    ssd1306_send_buffer(&Display);

    while (true) {
        printf("test\n");
        sleep_ms(1000);
    }

    return 0;
};
