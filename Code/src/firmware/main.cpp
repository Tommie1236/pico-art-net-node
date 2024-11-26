#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"

#include "main.h"

#include <DmxOutput.h>
//#include <DmxInput.h>
// Dmx input maybe be supported in the future. for now only output.
#include <ssd1306.h>
#include <textRenderer/TextRenderer.h>


using namespace pico_ssd1306;

DmxOutput dmx_out[2];
//DmxInput  dmx_in[2];


uint8_t universe_A[UNIVERSE_LENGTH + 1];
uint8_t universe_B[UNIVERSE_LENGTH + 1];








void init_gpio() {

    int mask = 0;
    mask |= (1 << PORT_A_DIR_PIN);
    mask |= (1 << PORT_A_LED_PIN);
    mask |= (1 << PORT_B_DIR_PIN);
    mask |= (1 << PORT_B_LED_PIN);
    mask |= (1 << MENU_BUTTON_PIN);
    mask |= (1 << UP_BUTTON_PIN);
    mask |= (1 << DOWN_BUTTON_PIN);
    mask |= (1 << EXIT_BUTTON_PIN);
    mask |= (1 << ETH_INT_PIN);
    mask |= (1 << ETH_RST_PIN);

    gpio_init_mask(mask);

    // ETH_INT_PIN is an input. The rest are outputs.
    // So remove the ETH_INT_PIN from the mask.
    mask &= ~(1 << ETH_INT_PIN);
    gpio_set_dir(ETH_INT_PIN, GPIO_IN);
    gpio_set_dir_out_masked(mask);

    gpio_set_drive_strength(PORT_A_LED_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_drive_strength(PORT_B_LED_PIN, GPIO_DRIVE_STRENGTH_12MA);
}

void init_oled() {
    i2c_init(i2c0, 100 * 1000);

    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
}

void main_core_1 () {
    // handle artnet connection and send the dmx data to the pio's
    // input isn't supported yet. only output.
}

int main () {
    // Handle user interaction: display/buttons. and the settings.

    stdio_init_all();
    init_gpio();
    init_oled();


    SSD1306 display = SSD1306(i2c0, 0x3c, Size::W128xH64);
    display.setOrientation(0);
    
    drawText(&display, font_12x16, "line 1 icons", 0, 0);
    drawText(&display, font_12x16, "line 2 menu", 0, 16);
    drawText(&display, font_12x16, "line 3 menu", 0, 32);
    drawText(&display, font_12x16, "line 4 menu", 0, 48);
    
    display.sendBuffer(); 
   
};