// config.c

#include "config.h"

#include "hardware/flash.h"


void config_save(config_t* config){
    
    // Make sure magic number is set.
    config->magic_number = CONFIG_MAGIC;

    // Disable interrupts to write to flash.
    uint32_t interupts = save_and_disable_interrupts();

    flash_range_erase(FLASH_TARGET_OFFSET, 4096);
    flash_range_program(FLASH_TARGET_OFFSET, (const uint8_t*)config, sizeof(config_t));

    restore_interrupts(interrupts);
}



void config_load(config_t* config){

    const uint32_t* flash_ptr = (const uint32_t*)(XIP_BASE + FLASH_TARGET_OFFSET);
    const config_t* flash_config = (const config_t*) flash_ptr;

    if (flash_config->magic_number == CONFIG_MAGIC) {
        memcpy(config, flash_config, sizeof(config_t));

    } else {
        // Fallback if no valid config in flash.
        config_reset(config);
    };
}



void config_reset(config_t* config){

    memset(config, 0, sizeof(config_t));

    config->ip[0] = 2;
    config->ip[1] = 0;
    config->ip[2] = 0;
    config->ip[3] = 10;

    config->subnet[0] = 255;
    config->subnet[1] = 0;
    config->subnet[2] = 0;
    config->subnet[3] = 0;

    // reset node name. padding at end to fill entire 18 bytes.
    memcpy(config->node_name, "Pico Artnet Node  ", 18); 
    
    config->port_A_status = OUTPUT;
    config->port_B_status = OUTPUT;
    config->port_A_universe = 0;
    config->port_0_universe = 1;
}





