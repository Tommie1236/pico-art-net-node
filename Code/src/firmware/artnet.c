// artnet.c
#include "artnet.h"

#include "dmx.h"
#include "main.h" // TODO: temp for led pin

#include <stdio.h>

#include <hardware/timer.h>
#include <pico/time.h>
#include <pico/unique_id.h>

#include <hardware/gpio.h>

#include <stdint.h>
#include <string.h>

static spi_inst_t *eth_spi_inst;
static uint8_t eth_cs_pin;
static uint8_t mac_address[6] = {0};

// ArtSync
static uint64_t last_sync = 0;
static uint8_t last_dmx_packet_ip[4] = {0};

// A artnet packet should be max
// 530 bytes, but adding a bit of extra buffer.
static uint8_t eth_buf[550] = {0}; 

static const uint8_t port_type_map[] = {
    [INPUT]    = 0x40,
    [OUTPUT]   = 0x80,
    [DISABLED] = 0x00,
};

void process_artnet(config_t *config) {

    // TODO: move out of method that is only called on every network irq.
    // disable synchronus output after 4 seconds of no artSync packet
    if (( time_us_64() - last_sync) > ( 4 * 1000 * 1000)) {
        config_set(config, sync_mode, false);
    }

    int16_t status = getSn_RX_RSR(0);

    if (status == 0) return;


    uint8_t source_ip[4] = {0};
    uint16_t source_port = 0;
    
    int32_t packet_len = recvfrom(0, eth_buf, sizeof(eth_buf), source_ip, &source_port);
    
#ifdef DEBUG_LOGGING
    printf("----------\n");
    printf("ip: %d.%d.%d.%d\n", config->ip[0], config->ip[1],config->ip[2],config->ip[3]);
    printf("status %u\n", status);
    printf("recieved packet from ip: %d.%d.%d.%d\n", source_ip[0], source_ip[1], source_ip[2], source_ip[3]);
#endif

    if ((packet_len > 550) || (packet_len < 12)) {
#ifdef DEBUG_LOGGING
        printf("packet too long or short. SKIP.\n");
#endif
        return;
    };

    // Artnet
    if (memcmp(eth_buf, "Art-Net\0", 8) == 0) {

#ifdef DEBUG_LOGGING
        printf("recieved art-package. code: %x\n", eth_buf[9]);
#endif

        switch (eth_buf[9]) {

            // artPoll
            case 0x20:  
                {

#ifdef DEBUG_LOGGING
                    printf("artpoll\n");
#endif

                    uint8_t tx_buf[207] = {0};
                    // uint8_t tx_buf[239] = {0}; // extended reply

                    // artpoll reply message
                    memcpy(tx_buf, "Art-Net\0", 8);
                    tx_buf[8] = 0x00;   // OpCode low 0x2100
                    tx_buf[9] = 0x21;   // OpCode high
                    memcpy(tx_buf + 10, config->ip, 4);    // Node Ip
                    tx_buf[14] = 0x36;  // Port low 0x1936 (6454)
                    tx_buf[15] = 0x19;  // Port high 
                    tx_buf[16] = 0x01;  // firmware version high
                    tx_buf[17] = 0x00;  // firmware version low
                    tx_buf[18] = 0x00;  // Netswitch high
                    tx_buf[19] = 0x00;  // Netswitch high
                    tx_buf[20] = 0x00;  // oem code high
                    tx_buf[21] = 0x00;  // oem code low
                    tx_buf[22] = 0x00;  // ubea Version
                    // TODO: update with rdm status
                    tx_buf[23] = (0b11 << 6) | (0b01 << 4); // | (0b1 << 1); // Status 1
                    //           Normal mode   front panel addr  rdm capable
                    tx_buf[24] = 0x00;  // ESTA code low (empty for hobby device)
                    tx_buf[25] = 0x00;  // high
                    memcpy(tx_buf + 26, config->node_name, 18);
                    memcpy(tx_buf + 44, config->long_node_name, 64);
                    // memcpy(tx_buf + 108, nodereport); // node status engineering
                    // tx_buf[172] = 0x00; // numports high (unused)
                    tx_buf[173] = 2;    // number of in or output ports.
                    tx_buf[174] = port_type_map[config->port_A_mode];
                    tx_buf[175] = port_type_map[config->port_B_mode];
                    // tx_buf[176] = 0x00; // there are no port 3/4 on the node
                    // tx_buf[177] = 0x00; // ^
                    // TODO: good in/output required merging and other status info
                    // memcpy(tx_buf + 178, goodInput, 4); // good input
                    // memcpy(tx_buf + 182, goodOutputA, 4); // good output A

                    // memcpy(tx_buf + 186, swin, 4); // Swin
                    // memcpy(tx_buf + 190, swout, 4); // SwOut
                    // tx_buf[194] = 0x00;     // sACN priority
                    // tx_buf[195] = 0x00;     // SwMacro
                    // tx_buf[196] = 0x00;     // SwRemote
                    // tx_buf[197] = 0x00;     // Spare
                    // tx_buf[198] = 0x00;     // Spare
                    // tx_buf[199] = 0x00;     // Spare
                    tx_buf[200] = 0x00;     // Style (default artnet node)
                    memcpy(tx_buf + 201, mac_address, 6);  // node mac address 

                    sendto(0, tx_buf, sizeof(tx_buf), source_ip, source_port);
                    break;
                }

            // artDMX
            case 0x50:
                if (config->sync_mode) memcpy(last_dmx_packet_ip, source_ip, 4);

                uint16_t length = (eth_buf[16] << 8 | eth_buf[17]);
                uint16_t universe = eth_buf[14] | (eth_buf[15] << 8);

#ifdef DEBUG_LOGGING
                uint8_t seq = eth_buf[12];

                printf("artdmx\n");
                printf("universe: %d\n", universe);
                printf("lenght: %d\n", length);
                printf("seq: %d\n", seq);
#endif

                if(universe == config->port_A_universe) {
                    gpio_put(PORT_A_LED_PIN, 0);
                    sleep_ms(10);
                    gpio_put(PORT_A_LED_PIN, 1);

                    while (dmx_busy()); 

                    // temporarely set eth_buf[17] to the dmx start code.
                    eth_buf[17] = 0x00;
                    dmx_write(eth_buf + 17, length+1);
                }


                // TODO: put correct dmx data vars here and uncomment
                //
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
                
                // return if artsync didn't come from same ip as last artdmx packet.
                if (memcmp(source_ip, last_dmx_packet_ip, 4) != 0) return;

                config_set(config, sync_mode, true);

                last_sync = time_us_64();

                // synced output dmx callback
                
                break;

            // artIpProg
            case 0xf8:
                {
                    uint8_t cmd = eth_buf[14];

                    if (cmd & 1 << 7U) {
                        // if (cmd & 1 << 6U); // NOTE: dhcp is not supported for now.
                        if (cmd & 1 << 4U) {
                            memcpy(config->gateway, eth_buf + 26, 4);
                            config->updated = true;
                        };
                        if (cmd & 1 << 3U) { // set ip/sn/port to default
                            memcpy(config->ip, CONFIG_DEFAULT_IP, 4);
                            memcpy(config->subnet, CONFIG_DEFAULT_SUBNET, 4);
                            // NOTE: using a port other than 6454 isn't supported right now.
                            // could be added pretty simply but has no usecase in normal systems. Won't add.
                            config->updated = true;
                        }
                        if (cmd & 1 << 2U) {
                            memcpy(config->ip, eth_buf + 16, 4);
                            config->updated = true;
                        };
                        if (cmd & 1 << 1U) {
                            memcpy(config->subnet, eth_buf + 20, 4);
                            config->updated = true;
                        };
                        // if (cmd & 1 << 0U); // NOTE: port is hardcoded for now, skip.
                        config_save(config);
                    }

                    uint8_t tx_buf[34] = {0};

                    memcpy(tx_buf, "Art-Net\0", 8);
                    // tx_buf[8] = 0x00;   // opcode high
                    tx_buf[9] = 0xf9;
                    tx_buf[10] = 0x00; // prot high
                    tx_buf[11] = 0x0E;  // prot low
                    // tx_buf[12] = 0x00; // Spare
                    // tx_buf[13] = 0x00; // Spare
                    // tx_buf[14] = 0x00; // Spare
                    memcpy(tx_buf + 16, config->ip, 4);
                    memcpy(tx_buf + 20, config->subnet, 4);
                    tx_buf[24] = 0x19; // portHi (deprecated).
                    tx_buf[25] = 0x36; // portLow
                    tx_buf[26] = 0x00;  // status (dhcp not supported so 0)
                    // tx_buf[27] = 0x00; // Spare
                    memcpy(tx_buf + 28, config->gateway, 4);
                    // tx_buf[32] = 0x00; // Spare
                    // tx_buf[33] = 0x00; // Spare
                    
                    sendto(0, tx_buf, sizeof(tx_buf), source_ip, source_port);

                    break;
                }
        } 

    } // else // TODO: sACN ?

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

wiz_NetInfo* setup_w5500(config_t *config,
                         spi_inst_t *spi_inst,
                         uint8_t sck_pin,
                         uint8_t mosi_pin,
                         uint8_t miso_pin,
                         uint8_t cs_pin,
                         uint8_t rst_pin,
                         uint8_t irq_pin) {

    eth_spi_inst = spi_inst;
    eth_cs_pin = cs_pin;

    gpio_init(rst_pin);
    gpio_set_dir(rst_pin, GPIO_OUT);
    gpio_put(rst_pin, 0);
    sleep_ms(10);
    gpio_put(rst_pin, 1);
    sleep_ms(50);

    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);
    spi_init(eth_spi_inst, 10 * 1000 * 1000); // 10 MHz
    spi_set_format(eth_spi_inst, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_init(eth_cs_pin);
    gpio_set_dir(eth_cs_pin, GPIO_OUT);
    gpio_put(eth_cs_pin, 1);

    reg_wizchip_spi_cbfunc(wizchip_spi_read, wizchip_spi_write);
    reg_wizchip_cs_cbfunc(wizchip_cs_select, wizchip_cs_deselect);

    // w5500 has 32kb memory for sockets. specify how to split.
    // for now, the normal split of 2kb per socket is more than enough
    uint8_t tx_rx_mem[2][8] = { {2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};

    wizchip_init(tx_rx_mem[0], tx_rx_mem[1]);

    static wiz_NetInfo netinfo;

    setup_network(config, &netinfo);
    socket(0, Sn_MR_UDP , 6454, 0);

    // init irq for socket 0 rx
    setSIMR(0x01);
    setSn_IMR(0, Sn_IR_RECV);

    gpio_set_irq_enabled_with_callback(irq_pin, GPIO_IRQ_EDGE_FALL, true, &w5500_irq_handler);

    return &netinfo;
}

void setup_network(config_t *config, wiz_NetInfo *net_info) {
    net_info->dhcp = NETINFO_STATIC; // NOTE: DHCP not supported.

    memcpy(net_info->ip, config->ip, 4);
    memcpy(net_info->sn, config->subnet, 4);
    memcpy(net_info->gw, config->gateway, 4);
    
    // generate mac addr from default base and serial number.
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);

    memcpy(mac_address, id.id, 6);
    mac_address[0] = (mac_address[0] & 0xfe) | 0x02;

    memcpy(net_info->mac, mac_address, 6);

    wizchip_setnetinfo(net_info);
}

void update_network(config_t *config, wiz_NetInfo *net_info) {
    // update the netInfo settings from the current internal config.
    if (config->updated) {
        memcpy(net_info->ip, config->ip, 4);
        memcpy(net_info->sn, config->subnet, 4);
        memcpy(net_info->gw, config->gateway, 4);
        wizchip_setnetinfo(net_info);
    }
}

void w5500_irq_handler(uint gpio, uint32_t events) {
    uint8_t irq = getSn_IR(0);

    if (irq & Sn_IR_RECV) {
        setSn_IR(0, Sn_IR_RECV); 
        // TODO: set data rx flag
    } // else? what about different irq?
}

