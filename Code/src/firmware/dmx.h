// dmx_out.h

// TODO: Can both input and output be in same file?
// or is splitting needed?

#ifndef DMX_OUT_H
#define DMX_OUT_H

#include "pico/stdlib.h"
#include "hardware/pio.h"

// general 
void dmx_init();

viod dmx_deinit();

// dmx output
void dmx_write();

bool dmx_busy();

// dmx input

void dmx_read();

void dmx_read_async();

uint32_t dmx_time_since_last_frame();




#endif // DMX_OUT_H
