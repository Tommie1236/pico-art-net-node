// Config.h

#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"

#define CONFIG_MAGIC 0x434E4647 // 'CNFG'
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - 4096)

typedef enum {
    INPUT,
    OUTPUT,
    DISABLED
} port_status_t;


typedef struct {
    uint8_t ip[4];
    uint8_t subnet[4];
    char node_name[18];
    char long_node_nage[64];
    port_status_t port_A_status;
    uint16_t port_A_universe;
    port_status_t port_B_status;
    uint16_t port_B_universe;
    uint32_t magic_number; // 0x434E4647 CNFG
    bool updated;
} config_t;



void config_save(config_t* config);
/* Saves the given config to flash.
 * 
 * Parameters:
 * - config (config_t)   
*/

void config_load(config_t* config);
/* Loads the given config with data from flash.
 * If corrupt or missing set to default.
 * 
 * Parameters:
 * - config (config_t)   
g*/

void config_reset(config_t* config);
/* Loads the given config to default.
 * 
 * Parameters:
 * - config (config_t)   
*/




#endif // CONFIG_H
