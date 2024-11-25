#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "main.h"

#include <DmxOutput.h>
//#include <DmxInput.h>
// Dmx input maybe be supported in the future. for now only output.
//#include <pico-ssd1306/ssd1306.h>
//#include <pico-ssd1306/textRenderer/TextRenderer.h>

//#include <hardware/i2c.h>


//using namespace pico_ssd1306;

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


int main () {
    stdio_init_all();
    init_gpio();
    dmx_out[PORT_A].begin(PORT_A_TX_PIN);
    //dmx_out[PORT_B].begin(PORT_B_TX_PIN);

    gpio_put(PORT_A_DIR_PIN, 1);
    gpio_put(PORT_A_LED_PIN, 1);

    //i2c_init(i2c0, 100 * 1000);
    //gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    //gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
    //gpio_pull_up(DISPLAY_SDA_PIN);
    //gpio_pull_up(DISPLAY_SCL_PIN);

    //sleep_ms(100); // wait for display to boot up

    //SSD1306 display = SSD1306(i2c0, 0x3D, Size::W128xH32);
    //display.setOrientation(0);
    //drawText(&display, font_16x32, "Hello World!", 0, 0);
    
    //display.sendBuffer();
    while (1) {}
};