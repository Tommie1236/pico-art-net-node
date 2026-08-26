// dmx_out.h

// TODO: Can both input and output be in same file?
// or is splitting needed?

#ifndef DMX_OUT_H
#define DMX_OUT_H

#include "config.h"

#include "hardware/pio.h"
#include "dmx_out.pio.h"
#include "main.h"

// NOTE: update memcpy in dmx_init if updating.
typedef struct {
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t dir_pin;
    uint8_t led_pin;
    int8_t sm;
    int8_t dma_ch;
} port_config_t;

typedef struct {
    port_config_t A;
    port_config_t B;
    PIO pio;
    uint8_t prog_tx_offset;
    uint8_t prog_rx_offset;
    uint32_t clk_div;
} dmx_config_t;

typedef enum {
    PORT_A,
    PORT_B
} dmx_port_t;

// general 
void dmx_init(config_t*,
              port_config_t*,
              port_config_t*,
              PIO);

void dmx_deinit();

void dmx_set_port_direction(dmx_port_t, port_mode_t);

// dmx output
void dmx_write(dmx_port_t, uint8_t *, uint16_t); //, uint8_t *data_b, uint16_t lenght_b);

bool dmx_busy(dmx_port_t);

// dmx input

void dmx_read();

void dmx_read_async();

uint32_t dmx_time_since_last_frame();




#endif // DMX_OUT_H
