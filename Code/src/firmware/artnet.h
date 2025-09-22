// artnet.h

// all the ethernet and artnet handeling.

#ifndef ARTNET_H
#define ARTNET_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "config.h"

#define _WIZCHIP_ W5500
#include "W5500/w5500.h"
#include "socket.h"



void process_artnet(config_t *config);

wiz_NetInfo* setup_w5500(spi_inst_t *eth_spi_inst,
                 uint8_t eth_sck_pin,
                 uint8_t eth_mosi_pin,
                 uint8_t eth_miso_pin,
                 uint8_t eth_cs_pin,
                 uint8_t eth_rst_pin
                 );

void setup_network(wiz_NetInfo *net_info);


#endif // ARTNET_d
