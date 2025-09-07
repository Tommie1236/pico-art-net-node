// artnet.c

#include "artnet.h"

#include "config.h"
#include "socket.h"

#include <hardware/timer.h>
#include <pico/time.h>
#include <pico/unique_id.h>

#include <hardware/gpio.h>

#include <stdint.h>
#include <string.h>

static spi_inst_t *eth_spi_inst;
static uint8_t eth_cs_pin;

static uint64_t last_sync = 0;

static uint8_t eth_buf[550] = {0}; 
// A artnet packet should be max
// 530 bytes, but adding a bit of extra buffer.

void process_artnet(config_t *config) {

    // disable synchronus output after 4 seconds of no artSync packet
    if (( time_us_64() - last_sync) > ( 4 * 1000 * 1000)) {
        config_set(config, sync_mode, false);
    }
    
    uint32_t plen = recvfrom(0, eth_buf, sizeof(eth_buf), NULL, NULL);
    
    // Artnet
    if (plen >= 12 && memcmp(eth_buf, "Art-Net\0", 8) == 0) {
       switch (eth_buf[8]) {

            // artPoll
            case 0x20:  
                uint8_t tx_buf[207];

                memcpy(tx_buf, "Art-Net\n", 8);
                //tx_buf[8] = 0x00;    // OpCode 0x2100
                tx_buf[9] = 0x21;   // OpCode
                memcpy(tx_buf + 10, config->ip, 4);    // Node Ip
                tx_buf[14] = 0x36;  // Port high 0x1936 (6454)
                tx_buf[15] = 0x19;  // Port low
                tx_buf[16] = 0x01;  // firmware version high
                //tx_buf[17] = 0x00;  // low
                // uint16 netswtichH/L?
                // uint16 OemH/L
                // tx_buf[22] = 0x00;  // ubea Version
                tx_buf[21] = (0b11 << 6) | (0b01 << 4); // Status
                // tx_buf[22]  // ESTA code low
                // tx_buf[23]  // high
                memcpy(tx_buf + 22, config->node_name, 18);
                memcpy(tx_buf + 40, config->long_node_name, 64);
                // memcpy(tx_buf + 104, nodereport); // node status engineering
                tx_buf[168] = 0x00;
                tx_buf[169] = 2;    // number of in or output ports.
                memcpy(tx_buf + 170, (uint8_t[]) {0xf0, 0xf0, 0x00, 0x00}, 4);

                
                break;
            
            // artDMX
            case 0x50:
                // uint8_t seq = eth_buf[12];

                // uint16_t length = (eth_buf[16] << 8 | eth_buf[17]);
                // uint16_t universe = eth_buf[14] | (eth_buf[15] << 8);

                // TODO: put correct dmx data vars here and uncomment
                // if (config->port_A_universe == universe) {
                //     memcpy(port_A_dmx_data, eth_buf + 18, lenght);
                //     port_A_updated = true;
                // }
                // if (config->port_B_universe == universe) {
                //     memcpy(port_B_dmx_data, eth_buf + 18, lenght);
                //     port_B_updated = true;
                // }
                break;

            // artSync
            case 0x52:
                // TODO:
                // compare/check ip with last artDmx packet.
                // ignore when not matching
                config_set(config, sync_mode, true);

                last_sync = time_us_64();

                // synced output dmx callback
                
                break;

            // artIpProg
            case 0xf8:
                break;
        } 

    } else // sACN ?
    if (0) {} 

}

// w5500 spi callbacks
static uint8_t wizchip_spi_read(void) {
    uint8_t data;
    spi_read_blocking(eth_spi_inst, 0xff, &data, 1);
    return data;
}

static void wizchip_spi_write(uint8_t data) {
    spi_write_blocking(eth_spi_inst, &data, 1);
}

static void wizchip_cs_select(void) {
    gpio_put(eth_cs_pin, 0);
}

static void wizchip_cs_deselect(void) {
    gpio_put(eth_cs_pin, 1);
}

void setup_w5500(spi_inst_t *spi_inst,
                 uint8_t sck_pin,
                 uint8_t mosi_pin,
                 uint8_t miso_pin,
                 uint8_t cs_pin,
                 uint8_t rst_pin) {
    
    eth_spi_inst = spi_inst;
    eth_cs_pin = cs_pin;

    gpio_init(rst_pin);
    gpio_set_dir(rst_pin, GPIO_OUT);
    gpio_put(rst_pin, 0);
    sleep_ms(10);
    gpio_put(rst_pin, 1);
    sleep_ms(50);

    spi_init(eth_spi_inst, 10 * 1000 * 1000); // 10 MHz
    spi_set_format(eth_spi_inst, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    gpio_init(eth_cs_pin);
    gpio_set_dir(eth_cs_pin, GPIO_OUT);
    gpio_put(eth_cs_pin, 1);

    reg_wizchip_spi_cbfunc(wizchip_spi_read, wizchip_spi_write);
    reg_wizchip_cs_cbfunc(wizchip_cs_select, wizchip_cs_deselect);


    socket(0, Sn_MR_UDP, 6465, 0);
}

void setup_network(wiz_NetInfo *net_info) {
    net_info->dhcp = NETINFO_STATIC;
    
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);

    memcpy(net_info->mac, id.id, 6);
    net_info->mac[0] = (net_info->mac[0] & 0xfe) | 0x02;

    wizchip_setnetinfo(net_info);
}

