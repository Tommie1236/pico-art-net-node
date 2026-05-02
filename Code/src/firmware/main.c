
#include "stdio.h"

#include "pico/time.h"

#include "main.h"

#include "ssd1306.h"
#include "artnet.h"
#include "input.h"
#include "menu.h"
#include "config.h"
#include "dmx.h"

// #include "ssd1306.h"
#include "hardware/gpio.h"

// int main() {
    // stdio_usb_init();
    // stdio_init_all();
 
    // sleep_ms(1000);

    // i2c_init(i2c0, 100 * 1000);
 
    // gpio_init(8);
    // gpio_init(9);
 
    // gpio_set_function(8, GPIO_FUNC_I2C);
    // gpio_set_function(9, GPIO_FUNC_I2C);
 
    // gpio_pull_up(8);
    // gpio_pull_up(9);
 
    // ssd1306_display_t Display;
 
    // ssd1306_init(&Display, i2c0, 0x3c, 128, 64);
    // ssd1306_draw_line(&Display, 0, 0, 128, 64, ADD);
    // ssd1306_send_buffer(&Display);
 
    // while (true) {
        // printf("test\n");
        // sleep_ms(1000);
    // }
 
    // return 0;
// };


int main() {
    stdio_init_all();
    sleep_ms(20);

    config_t config;

    // loads config from flash if available. othewise resets.
    config_load(&config);

    //wiz_NetInfo *netinfo = setup_w5500(
    setup_w5500(
        &config,
        ETH_SPI,
        ETH_SCK_PIN,
        ETH_MOSI_PIN,
        ETH_MISO_PIN,
        ETH_CS_PIN,
        ETH_RST_PIN);


    printf("ip: %d.%d.%d.%d\n", config.ip[0], config.ip[1],config.ip[2],config.ip[3]);
    stdio_flush();

    gpio_init(PORT_A_DIR_PIN);
    gpio_set_dir(PORT_A_DIR_PIN, GPIO_OUT);
    gpio_put(PORT_A_DIR_PIN, 1);

    dmx_init(PORT_A_TX_PIN, pio0, PORT_B_TX_PIN, pio0);

    sleep_ms(100);

    gpio_init(PORT_A_LED_PIN);
    gpio_set_dir(PORT_A_LED_PIN, GPIO_OUT);
    gpio_init(PORT_B_LED_PIN);
    gpio_set_dir(PORT_B_LED_PIN, GPIO_OUT);
    gpio_put(PORT_A_LED_PIN, 1);
    gpio_put(PORT_B_LED_PIN, 1);
    for (;;) {
        process_artnet(&config);
        // gpio_put(PORT_A_LED_PIN, 1);
        // gpio_put(PORT_B_LED_PIN, 0);
        // sleep_ms(500);
        // gpio_put(PORT_A_LED_PIN, 0);
        // gpio_put(PORT_B_LED_PIN, 1);
        // sleep_ms(500);
    }
    return 0;
}
