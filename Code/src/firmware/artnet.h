// artnet.h
// all the ethernet and artnet handeling.

#ifndef ARTNET_H
#define ARTNET_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define _WIZCHIP_ W5500
#include "W5500/w5500.h"
#include "wizchip_conf.h"
#include "socket.h"



void process_artnet(uint8_t *buffer);

void setup_w5500(spi_inst_t eth_spi_inst,
                 uint8_t eth_sck_pin,
                 uint8_t eth_mosi_pin,
                 uint8_t eth_miso_pin,
                 uint8_t eth_cs_pin,
                 uint8_t eth_rst_pin
                 );

void setup_network(wiz_NetInfo net_info);

// needed?
void set_ip(
    wiz_NetInfo net_info,
    uint8_t ip[4]);

void set_subnet(
    wiz_NetInfo net_info,
    uint8_t subnet[4]);

void set_gateway(
    wiz_NetInfo net_info,
    uint8_t gateway[4]);

void set_dns(
    wiz_NetInfo net_info,
    uint8_t dns[4]);



#endif // ARTNET_d
