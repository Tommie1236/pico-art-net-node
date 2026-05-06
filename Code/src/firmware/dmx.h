// dmx_out.h

// TODO: Can both input and output be in same file?
// or is splitting needed?

#ifndef DMX_OUT_H
#define DMX_OUT_H

#include "config.h"

#include "hardware/pio.h"
#include "dmx_out.pio.h"

// general 
void dmx_init(config_t *config, uint16_t pin_A, PIO Pio_A, uint16_t pin_B, PIO Pio_B);

void dmx_deinit();

// dmx output
void dmx_write(uint8_t *data_a, uint16_t lenght_a); //, uint8_t *data_b, uint16_t lenght_b);

bool dmx_busy();

// dmx input

void dmx_read();

void dmx_read_async();

uint32_t dmx_time_since_last_frame();




#endif // DMX_OUT_H
