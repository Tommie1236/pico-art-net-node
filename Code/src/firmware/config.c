// config.c

#include "config.h"

#include <string.h>

#include "hardware/flash.h"
#include "hardware/irq.h"
#include "pico/critical_section.h" // interrupts

void config_save(config_t* config){

    if (!config->updated) { // don't do anything if the config didn't change
        return;
    }
    
    // Make sure magic number is set.
    config->magic_number = CONFIG_MAGIC;

    // Disable interrupts to write to flash.
    uint32_t interrupts = save_and_disable_interrupts();

    flash_range_erase(FLASH_TARGET_OFFSET, 4096);
    flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t*)config, sizeof(config_t));

    restore_interrupts(interrupts);
}



void config_load(config_t* config){

    const uint32_t* flash_ptr = (const uint32_t*)(XIP_BASE + FLASH_TARGET_OFFSET);
    const config_t* flash_config_ptr = (const config_t*) flash_ptr;

    if (flash_config_ptr->magic_number == CONFIG_MAGIC) {
        memcpy(config, flash_config_ptr, sizeof(config_t));
    } else {
        // Fallback if no valid config in flash.
        config_reset(config);
    };
}



void config_reset(config_t* config){

    memset(config, 0, sizeof(config_t));

    memcpy(config->ip, (uint8_t[]) {192, 168, 2, 230}, 4);
    memcpy(config->subnet, (uint8_t[]) {255, 255, 255, 0}, 4);
    // gateway isn't really used but is still reset.
    memcpy(config->gateway, (uint8_t[]) {0, 0, 0, 0}, 4);

    // reset names to blank and copy in default name.
    memset(config->node_name, ' ', 18);
    memcpy(config->node_name, "Pico Artnet Node", 16); 
    memset(config->long_node_name, ' ', 64);
    memcpy(config->long_node_name, "Pico Artnet Node", 16);
    
    config->port_A_status = OUTPUT;
    config->port_B_status = OUTPUT;
    config->port_A_universe = 0;
    config->port_B_universe = 1;

    config->sync_mode = false;

    config->updated = true;
}

