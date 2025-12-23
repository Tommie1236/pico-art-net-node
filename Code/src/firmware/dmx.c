


#include "dmx.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include <hardware/pio.h>
#include <hardware/dma.h>

static uint16_t pin_A;
static uint16_t pin_B;
static PIO pio_A;
static PIO pio_B;
static uint8_t sm_A;
// static uint8_t sm_B;
static uint8_t dma_ch_A;
// static uint8_t dma_ch_B;

static uint prog_offset;



void dmx_init(uint16_t pinA, PIO PioA, uint16_t pinB, PIO PioB) {
    pin_A = pinA;
    pin_B = pinB;
    pio_A = PioA;
    pio_B = PioB;

    // PIO
    prog_offset = pio_add_program(pio_A, &dmx_output_program);
    sm_A = pio_claim_unused_sm(pio_A, false);

    pio_sm_set_pins_with_mask(pio_A, sm_A, 1u << pin_A, 1u << pin_A);
    pio_sm_set_pindirs_with_mask(pio_A, sm_A, 1u << pin_A, 1u << pin_A);
    pio_gpio_init(pio_A, pin_A);

    pio_sm_config sm_conf = dmx_output_program_get_default_config(prog_offset);

    sm_config_set_out_pins(&sm_conf, pin_A, 1);
    sm_config_set_sideset_pins(&sm_conf, pin_A);

    uint clk_div = clock_get_hz(clk_sys) / 1000000;
    sm_config_set_clkdiv(&sm_conf, clk_div);

    pio_sm_init(pio_A, sm_A, prog_offset, &sm_conf);
    pio_sm_set_enabled(pio_A, sm_A, true);

    // DMA
    dma_ch_A = dma_claim_unused_channel(false);
    
    dma_channel_config dma_conf = dma_channel_get_default_config(dma_ch_A);
    channel_config_set_transfer_data_size(&dma_conf, DMA_SIZE_8);

    channel_config_set_dreq(&dma_conf, pio_get_dreq(pio_A, sm_A, true));

    dma_channel_set_write_addr(dma_ch_A, &pio_A->txf[sm_A], false);

    dma_channel_set_config(dma_ch_A, &dma_conf, false);
}

void dmx_deinit() {

}

// dmx output
void dmx_write(uint8_t *data_a, uint16_t lenght_a) { //, uint8_t *data_b, uint16_t lenght_b) {

    pio_sm_set_enabled(pio_A, sm_A, false);

    pio_sm_restart(pio_A, sm_A);

    pio_sm_exec(pio_A, sm_A, pio_encode_jmp(prog_offset));

    pio_sm_set_enabled(pio_A, sm_A, true);

    dma_channel_transfer_from_buffer_now(dma_ch_A, data_a, lenght_a);
}

bool dmx_busy() {

    if(dma_channel_is_busy(dma_ch_A)) {
        return true;
    } 
    return !pio_sm_is_tx_fifo_empty(pio_A, sm_A);
}

// dmx input

void dmx_read() {

}

void dmx_read_async() {

}

uint32_t dmx_time_since_last_frame() {
    return 0;

}
