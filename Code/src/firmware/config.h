// Config.h

#ifndef CONFIG_H
#define CONFIG_H

#include "pico/stdlib.h"

#define CONFIG_MAGIC 0x434E4647 // 'CNFG'
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - 4096)

#define config_set(_config_ptr, _member, _value)                    \
    do {                                                            \
        __typeof__((_config_ptr)->_member) __new_value = (_value);  \
        if ((_config_ptr)->_member != __new_value) {                \
            (_config_ptr)->_member = __new_value;                   \
            (_config_ptr)->updated = true;                          \
        }                                                           \
    } while (0);

// Enum values map directly to dir pin output to max485
typedef enum {
    OUTPUT = 1,
    INPUT = 0,
    DISABLED = 2
} port_mode_t;


typedef struct {
    uint8_t ip[4];
    uint8_t subnet[4];
    uint8_t gateway[4];
    char node_name[18];
    char long_node_name[64];
    port_mode_t port_A_mode;
    uint16_t port_A_universe;
    port_mode_t port_B_mode;
    uint16_t port_B_universe;
    uint32_t magic_number; // 0x434E4647 CNFG
    bool sync_mode;
    bool updated;
} config_t;


void config_save(config_t* config);
/* Saves the given config to flash.
 * 
 * Parameters:
 * - config (config_t)   
*/

bool config_load(config_t* config);
/* Loads the given config with data from flash.
 * If corrupt or missing set to default.
 * 
 * Parameters:
 * - config (config_t)   
 *
 * Returns:
 *   1 when loading from flash
 *   0 when resetting config (no valid flash available)
g*/

void config_reset(config_t* config);
/* Loads the given config to default.
 * 
 * Parameters:
 * - config (config_t)   
*/




#endif // CONFIG_H
