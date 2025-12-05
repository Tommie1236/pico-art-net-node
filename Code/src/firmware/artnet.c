// artnet.c
#include "W5500/w5500.h"
#include "main.h" // TODO: temp for led pin

#include "artnet.h"
#include <stdio.h>

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

static uint8_t mac_address[6] = {0};

// static uint8_t last_dmx_packet_ip[4] = {0};

void process_artnet(config_t *config) {

    // disable synchronus output after 4 seconds of no artSync packet
    if (( time_us_64() - last_sync) > ( 4 * 1000 * 1000)) {
        config_set(config, sync_mode, false);
    }

    int16_t status = getSn_RX_RSR(0);

    if (status == 0) return;

    printf("----------\n");
    printf("ip: %d.%d.%d.%d\n", config->ip[0], config->ip[1],config->ip[2],config->ip[3]);
    printf("status %u\n", status);

    uint8_t source_ip[4] = {0};
    uint16_t source_port = 0;
    
    int32_t plen = recvfrom(0, eth_buf, sizeof(eth_buf), source_ip, &source_port);
    
    printf("recieved packet from ip: %d.%d.%d.%d\n", source_ip[0], source_ip[1], source_ip[2], source_ip[3]);

    if ((plen > 550) || (plen < 12)) {
        printf("packet too long or short\n");
    };
    
    // Artnet
    if (plen >= 12 && (memcmp(eth_buf, "Art-Net\0", 8) == 0)) {
        printf("recieved art-package. code: %x\n", eth_buf[9]);
        switch (eth_buf[9]) {

            // artPoll
            case 0x20:  
                printf("artpoll\n");
                // uint8_t tx_buf[207] = {0};
                uint8_t tx_buf[239] = {0};

                // artpoll reply message
                // TODO: check if all commented lines are really not needed/zeroed

                memcpy(tx_buf, "Art-Net\n", 8);
                tx_buf[8] = 0x00;   // OpCode low 0x2100
                tx_buf[9] = 0x21;   // OpCode high
                memcpy(tx_buf + 10, config->ip, 4);    // Node Ip
                tx_buf[14] = 0x36;  // Port low 0x1936 (6454)
                tx_buf[15] = 0x19;  // Port high 
                tx_buf[16] = 0x01;  // firmware version high
                tx_buf[17] = 0x00;  // firmware version low
                // tx_buf[18] = 0x00;  // Netswitch high
                // tx_buf[19] = 0x00;  // Netswitch high
                // tx_buf[20] = 0x00;  // oem code high
                // tx_buf[21] = 0x00;  // oem code low
                // tx_buf[22] = 0x00;  // ubea Version
                tx_buf[23] = 0x20;
                // tx_buf[23] = (0b11 << 6) | (0b01 << 4); // Status 1
                //           Normal mode   front panel addr
                tx_buf[24] = 0x00;  // ESTA code low
                tx_buf[25] = 0x00;  // high
                memcpy(tx_buf + 26, config->node_name, 18);
                memcpy(tx_buf + 44, config->long_node_name, 64);
                // memcpy(tx_buf + 108, nodereport); // node status engineering
                //tx_buf[172] = 0x00; // numports high (unused)
                tx_buf[173] = 2;    // number of in or output ports.
                memcpy(tx_buf + 174, (uint8_t[]) {0xf0, 0xf0, 0x00, 0x00}, 4);
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

                // uint8_t sr = getSn_SR(0);
                // printf("Socket SR: 0x%02x\n", sr); // should be SOCK_UDP = 0x22



                setSn_DIPR(0, source_ip);
                setSn_DPORT(0, source_port);

                // int32_t return_val = sendto(0, tx_buf, sizeof(tx_buf), source_ip, 6454);

                wiz_send_data(0, tx_buf, sizeof(tx_buf));

                setSn_CR(0, Sn_CR_SEND);
                uint32_t timeout_us = 10000; // 10ms timeout is usually sufficient
                while (!(getSn_IR(0) & Sn_IR_SENDOK) && (timeout_us > 0)) {
                    sleep_us(1); 
                    timeout_us--;
                }

                if (getSn_IR(0) & Sn_IR_SENDOK) {
                    setSn_IR(0, Sn_IR_SENDOK); // Clear it
                    printf("ArtPollReply sent via manual SEND.\n");
                    // return_val = packet_len; // Indicate success
                } else {
                    printf("Manual SEND failed/timed out.\n");
                    // return_val = -1; // Indicate failure
                }

                uint8_t dipr[4] = {0, 0, 0, 0};
                setSn_DIPR(0, dipr);
                setSn_DPORT(0, 0);

                // uint8_t dest_ip_check[4] = {0};
                // getSn_DIPR(0, dest_ip_check);
                // uint16_t dport = getSn_DPORT(0);
                // printf("DEBUG: Sn_DIPR is: %d.%d.%d.%d:%d\n", dest_ip_check[0], dest_ip_check[1], dest_ip_check[2], dest_ip_check[3], dport);
                // uint8_t temp_ip[4] = {192, 168, 2, 100};
                // int32_t return_val = sendto(0, tx_buf, sizeof(tx_buf), temp_ip, 6454);




                // int32_t conn_ret = connect(0, source_ip, source_port);

                // if (conn_ret != SOCK_OK) { 
                    // printf("Connect failed before send: %ld. Status: 0x%02x\n", conn_ret, getSn_SR(0));
                // }

                // int32_t return_val = send(0, tx_buf, sizeof(tx_buf));

                // disconnect(0);


                /*
                if (return_val < 0) {
                    printf("sendto failed: %ld\n", return_val);
                } else {
                    uint8_t ir = getSn_IR(0);
                    printf("sendto return_val=%ld, Sn_IR=0x%02x\n", return_val, ir);
                    // check SENDOK bit
                    if (ir & Sn_IR_SENDOK) {
                        printf("SEND OK\n");
                        setSn_IR(0, Sn_IR_SENDOK); // clear it
                    } else if (ir & Sn_IR_TIMEOUT) {
                        printf("SEND TIMEOUT\n");
                        setSn_IR(0, Sn_IR_TIMEOUT); // clear it
                    } else {
                        printf("No SENDOK/TIMEOUT — packet may be queued, check ARP\n");
                    }
                }
                */

                break;
            
            // artDMX
            case 0x50:
                printf("artdmx\n");

                // TODO: temp packet indicator, remove!
                gpio_put(PORT_A_LED_PIN, 0);
                sleep_ms(10);
                gpio_put(PORT_A_LED_PIN, 1);


                uint8_t seq = eth_buf[12];


                uint16_t length = (eth_buf[16] << 8 | eth_buf[17]);
                uint16_t universe = eth_buf[14] | (eth_buf[15] << 8);

                printf("universe: %d\n", universe);
                printf("lenght: %d\n", length);
                printf("seq: %d\n", seq);
                printf("ch 1 value: %d\n", eth_buf[18]);

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
                // TODO:
                // compare/check ip with last artDmx packet.
                // ignore when not matching
                config_set(config, sync_mode, true);

                last_sync = time_us_64();

                // synced output dmx callback
                
                break;

            // artIpProg
            case 0xf8:
                if (eth_buf[14] & 0b1) { // set port
                    // hardcoded to 6465 for now

                }
                if (eth_buf[14] & (0b1 << 1)) { // set subnet
                    memcpy(config->subnet, eth_buf + 20, 4);
                    config->updated = true;
                }
                if (eth_buf[14] & (0b1 << 2)) { // set ip addr
                    memcpy(config->ip, eth_buf + 16, 4);
                    config->updated = true;
                }
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

wiz_NetInfo* setup_w5500(config_t *config,
                         spi_inst_t *spi_inst,
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

    uint8_t tx_rx_mem[2][8] = { {2,2,2,2,2,2,2,2}, {2,2,2,2,2,2,2,2}};

    wizchip_init(tx_rx_mem[0], tx_rx_mem[1]);

    socket(0, Sn_MR_UDP | Sn_MR_MULTI, 6454, 0);



    // clear broadcast disable flag
    /*
    uint8_t sn_mr = getSn_MR(0);
    sn_mr &= ~Sn_MR_BCASTB;
    sn_mr &= ~Sn_MR_MMB;
    setSn_MR(0, sn_mr);
    */

    static wiz_NetInfo netinfo;

    setup_network(config, &netinfo);
    return &netinfo;
}

void setup_network(config_t *config, wiz_NetInfo *net_info) {
    net_info->dhcp = NETINFO_STATIC;

    memcpy(net_info->ip, config->ip, 4);
    memcpy(net_info->sn, config->subnet, 4);
    memcpy(net_info->gw, config->gateway, 4);
    
    
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);

    memcpy(mac_address, id.id, 6);
    mac_address[0] = (mac_address[0] & 0xfe) | 0x02;

    memcpy(net_info->mac, mac_address, 6);

    wizchip_setnetinfo(net_info);
}

