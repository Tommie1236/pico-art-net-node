
#include "stdio.h"

#include "pico/time.h"

#include "main.h"

// #include "ssd1306.h"
#include "artnet.h"
#include "input.h"
// #include "menu.h"
#include "config.h"
#include "dmx.h"


int main() {
#ifdef DEBUG_LOGGING
    stdio_init_all();
    sleep_ms(20);
#endif 

    config_t config;

    // loads config from flash if available. othewise resets.
    config_load(&config);

    //wiz_NetInfo *netinfo = setup_w5500(
    wiz_NetInfo *netinfo = setup_w5500(
        &config,
        ETH_SPI,
        ETH_SCK_PIN,
        ETH_MOSI_PIN,
        ETH_MISO_PIN,
        ETH_CS_PIN,
        ETH_RST_PIN,
        ETH_IRQ_PIN);

#ifdef DEBUG_LOGGING
    printf("ip: %d.%d.%d.%d\n", config.ip[0], config.ip[1],config.ip[2],config.ip[3]);
#endif 
    stdio_flush();

    dmx_init(&config, PORT_A_TX_PIN, pio0, PORT_B_TX_PIN, pio0);

    for (;;) {
        process_artnet(&config);
        // TODO: optimize by not checking if network settings have changed every loop.
        // change to work with callbacks etc.
        update_network(&config, netinfo);
    }
    return 0;
}
