


#include "dmx.h"
#include "main.h"

#include "stdio.h"

#include "hardware/clocks.h"
#include <hardware/pio.h>
#include <hardware/dma.h>

#include "string.h"

static dmx_config_t dmx_config;

void dmx_init(config_t *config,
              port_config_t *portA,
              port_config_t *portB,
              PIO Pio) {

    // TODO: Dmx input

    dmx_config.pio = Pio;
    dmx_config.clk_div = clock_get_hz(clk_sys) / 1000000;

    memcpy(&dmx_config.A, portA, 4);
    memcpy(&dmx_config.B, portB, 4);

    gpio_init(dmx_config.A.led_pin);
    gpio_init(dmx_config.A.dir_pin);
    gpio_init(dmx_config.B.led_pin);
    gpio_init(dmx_config.B.dir_pin);

    gpio_set_dir(dmx_config.A.led_pin, GPIO_OUT);
    gpio_set_dir(dmx_config.A.dir_pin, GPIO_OUT);
    gpio_set_dir(dmx_config.B.led_pin, GPIO_OUT);
    gpio_set_dir(dmx_config.B.dir_pin, GPIO_OUT);

    gpio_put(dmx_config.A.dir_pin, config->port_A_mode == DISABLED ? 0 : config->port_A_mode);
    gpio_put(dmx_config.A.led_pin, 1);

    gpio_put(dmx_config.B.dir_pin, config->port_B_mode == DISABLED ? 0 : config->port_B_mode);
    gpio_put(dmx_config.B.led_pin, 1);

    // PIO
    dmx_config.prog_tx_offset = pio_add_program(dmx_config.pio, &dmx_output_program);
    // dmx_config.prog_rx_offset = pio_add_program(dmx_config.pio, &dmx_input_program);

    dmx_config.A.sm = pio_claim_unused_sm(dmx_config.pio, false);
    dmx_config.B.sm = pio_claim_unused_sm(dmx_config.pio, false);
    dmx_config.A.dma_ch = dma_claim_unused_channel(false);
    dmx_config.B.dma_ch = dma_claim_unused_channel(false);

    if ((dmx_config.A.sm < 0 ) ||
        (dmx_config.B.sm < 0 )) {
#ifdef DEBUG_LOGGING
        printf("ERROR: SM config failed: not enough available in pio:%d", (uint)dmx_config.pio);
#endif
        return;
    }

    if ((dmx_config.A.dma_ch < 0 ) ||
        (dmx_config.B.dma_ch < 0 )) {
#ifdef DEBUG_LOGGING
        printf("ERROR: DMA config failed: not enough available dma channels for dmx");
#endif
        return;
    }

    pio_gpio_init(dmx_config.pio, dmx_config.A.tx_pin);
    pio_gpio_init(dmx_config.pio, dmx_config.A.rx_pin);
    pio_gpio_init(dmx_config.pio, dmx_config.B.tx_pin);
    pio_gpio_init(dmx_config.pio, dmx_config.B.rx_pin);

    dmx_set_port_direction(PORT_A, config->port_A_mode);
    dmx_set_port_direction(PORT_B, config->port_B_mode);
}

void dmx_deinit() {
    pio_sm_set_enabled(dmx_config.pio, dmx_config.A.sm, false);
    pio_sm_set_enabled(dmx_config.pio, dmx_config.B.sm, false);
    dma_channel_abort(dmx_config.A.dma_ch);
    dma_channel_abort(dmx_config.B.dma_ch);
    pio_sm_unclaim(dmx_config.pio, dmx_config.A.sm);
    pio_sm_unclaim(dmx_config.pio, dmx_config.B.sm);
    dma_channel_unclaim(dmx_config.A.dma_ch);
    dma_channel_unclaim(dmx_config.B.dma_ch);
}

void dmx_set_port_direction(dmx_port_t port, port_mode_t direction){
    // TODO: wait until port is idle to change. don't corrupt the dmx data and have glitches

    // Cleanup port but leave program in pio for later use. (And the other port could still be using it.)
    port_config_t *dmx_port= (port == PORT_A ? &dmx_config.A : &dmx_config.B);

    pio_sm_set_enabled(dmx_config.pio, dmx_port->sm, false);

    // Setup port

    // sets direction to input when disabled to not touch the dmx bus.
    gpio_put(dmx_port->dir_pin, direction == DISABLED ? 0 : direction);
    
    if (direction == INPUT) { return;
        // TODO: input
    } else if (direction == OUTPUT) {
        pio_sm_set_pindirs_with_mask(dmx_config.pio, dmx_port->sm, 1U << dmx_port->tx_pin, 1U << dmx_port->tx_pin);
        // Set dmx line to idle (high)
        pio_sm_set_pins_with_mask(dmx_config.pio, dmx_port->sm, 1U << dmx_port->tx_pin, 1U << dmx_port->tx_pin);

        pio_sm_config sm_config = dmx_output_program_get_default_config(dmx_config.prog_tx_offset);

        sm_config_set_out_pins(&sm_config, dmx_port->tx_pin, 1);
        sm_config_set_sideset_pins(&sm_config, dmx_port->tx_pin);

        sm_config_set_clkdiv(&sm_config, dmx_config.clk_div);

        pio_sm_init(dmx_config.pio, dmx_port->sm, dmx_config.prog_tx_offset + dmx_output_wrap_target, &sm_config);
        pio_sm_set_enabled(dmx_config.pio, dmx_port->sm, true);

        // DMA 
        dma_channel_config dma_config = dma_channel_get_default_config(dmx_port->dma_ch);
        channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_8);

        channel_config_set_dreq(&dma_config, pio_get_dreq(dmx_config.pio, dmx_port->sm, true));

        dma_channel_set_write_addr(dmx_port->dma_ch, &dmx_config.pio->txf[dmx_port->sm], false);

        dma_channel_set_config(dmx_port->dma_ch, &dma_config, false);

    } else return;
}


// dmx output
void dmx_write(uint8_t *data_a, uint16_t lenght_a) { //, uint8_t *data_b, uint16_t lenght_b) {
    // TODO: support 2nd output port

    pio_sm_set_enabled(dmx_config.pio, dmx_config.A.sm, false);

    pio_sm_restart(dmx_config.pio, dmx_config.A.sm);

    pio_sm_exec(dmx_config.pio, dmx_config.A.sm, pio_encode_jmp(dmx_config.prog_tx_offset));

    pio_sm_set_enabled(dmx_config.pio, dmx_config.A.sm, true);

    dma_channel_transfer_from_buffer_now(dmx_config.A.dma_ch, data_a, lenght_a);
}

bool dmx_busy() {

    if(dma_channel_is_busy(dmx_config.A.dma_ch)) {
        return true;
    } 
    return !pio_sm_is_tx_fifo_empty(dmx_config.pio, dmx_config.A.sm);
}

// dmx input
// TODO: supoprt dmx input

void dmx_read() {
    // reads blocking until next packet arrives

}

void dmx_read_async() {

}

uint32_t dmx_time_since_last_frame() {
    return 0;
}
