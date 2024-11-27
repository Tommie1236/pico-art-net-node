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
#include <shapeRenderer/ShapeRenderer.h>


using namespace pico_ssd1306;

#define FONT font_12x16


//DmxOutput dmx_out[2];
//DmxInput  dmx_in[2];


//uint8_t universe_A[UNIVERSE_LENGTH + 1];
//uint8_t universe_B[UNIVERSE_LENGTH + 1];



// Menu structure (https://www.tldraw.com/ro/jS2uV4zSO97DEFvRHu4y5)
//
// MAIN 
//  - Network
//      - ip
//          view current ip
//          - dhcp
//              enable
//              - static
//                  set ip
//                  set subnet
//  - Ports
//      - A
//      - B
//          - status
//              enable
//              disable
//          - direction
//              output
//              input       // maybe support later
//          - universe
//              net
//              subnet
//              universe
//  - Status ??
//
//

enum MENU_PAGE {
    MAIN,
    NETWORK,
    IP,
    DHCP, 
    DHCP_STATIC,
    PORTS,
    A,
    A_STATUS,
    A_DIRECTION,
    A_UNIVERSE,
    B,
    B_STATUS,
    B_DIRECTION,
    B_UNIVERSE,
    STATUS,
    LOCK            // Lock/unlock the menu by holding the menu button for 3 seconds.
};



void init_gpio() {

    uint32_t mask_all = 0;
    mask_all |= (1 << PORT_A_DIR_PIN);
    mask_all |= (1 << PORT_A_LED_PIN);
    mask_all |= (1 << PORT_B_DIR_PIN);
    mask_all |= (1 << PORT_B_LED_PIN);
    mask_all |= (1 << MENU_BUTTON_PIN);
    mask_all |= (1 << UP_BUTTON_PIN);
    mask_all |= (1 << DOWN_BUTTON_PIN);
    mask_all |= (1 << EXIT_BUTTON_PIN);
    mask_all |= (1 << ETH_INT_PIN);
    mask_all |= (1 << ETH_RST_PIN);

    uint32_t mask_inp = 0;
    mask_inp |= (1 << MENU_BUTTON_PIN);
    mask_inp |= (1 << UP_BUTTON_PIN);
    mask_inp |= (1 << DOWN_BUTTON_PIN);
    mask_inp |= (1 << EXIT_BUTTON_PIN);
    mask_inp |= (1 << ETH_INT_PIN);

    uint32_t mask_out = mask_all & ~mask_inp;

    gpio_init_mask(mask_all);
    gpio_set_dir_in_masked(mask_inp);
    gpio_set_dir_out_masked(mask_out);

    // there is no masked version of pullup/down.
    gpio_pull_up(MENU_BUTTON_PIN);
    gpio_pull_up(UP_BUTTON_PIN);
    gpio_pull_up(DOWN_BUTTON_PIN);
    gpio_pull_up(EXIT_BUTTON_PIN);
    gpio_pull_up(ETH_INT_PIN);      // needed for interrupt? will figure out later

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

void draw_menu_base(pico_ssd1306::SSD1306 *ssd1306) {
    ssd1306->clear();
    drawLine(ssd1306, 0, 15, 128, 15);
    // TODO: draw status icons
}

void main_core_1 () {
    // TODO:
    // handle artnet connection and send the dmx data to the pio's
    // input isn't supported yet. only output.
}

int main () {
    // Handle user interaction: display/buttons. and the settings.

    stdio_init_all();
    init_gpio();
    init_oled();
    
    gpio_put(PORT_A_LED_PIN, 1);


    SSD1306 display = SSD1306(i2c0, 0x3c, Size::W128xH64);
    display.setOrientation(0);
    display.clear();
    
    drawText(&display, font_12x16, "line 1 icons", 0, 0);
    drawText(&display, font_12x16, "line 2 menu", 0, 16);
    drawText(&display, font_12x16, "line 3 menu", 0, 32);
    drawText(&display, font_12x16, "line 4 menu", 0, 48);
    
    display.sendBuffer(); 



    // ---------

    
    MENU_PAGE current_page = MENU_PAGE::MAIN;
    uint8_t current_selection = 0;
    
    draw_menu_base(&display);
    while (true) {

    bool button_menu_pressed = !gpio_get(MENU_BUTTON_PIN);
    bool button_up_pressed = !gpio_get(UP_BUTTON_PIN);
    bool button_down_pressed = !gpio_get(DOWN_BUTTON_PIN);
    bool button_exit_pressed = !gpio_get(EXIT_BUTTON_PIN);

    switch (current_page) {

        case MENU_PAGE::MAIN:
            if (button_down_pressed) {
                if (current_selection < 2) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 2;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        current_page = MENU_PAGE::NETWORK;
                        current_selection = 0;
                        break;
                    case 1:
                        current_page = MENU_PAGE::PORTS;
                        current_selection = 0;
                        break;
                    case 2:
                        current_page = MENU_PAGE::STATUS;
                        current_selection = 0;
                        break;
                }
            } 
            draw_menu_base(&display);
            drawText(&display, FONT, ">", 0, (current_selection + 1) * 16);
            drawText(&display, FONT, "Network", 16, 16);
            drawText(&display, FONT, "Ports", 16, 32);
            drawText(&display, FONT, "Status", 16, 48);
            switch(current_selection) {
                case 0:
                    // TODO: invert "network"
                case 1:
                    // TODO: invert "ports"
                case 2:
                    // TODO: invert "status"
            display.sendBuffer(); 
            }
            break;

        case MENU_PAGE::NETWORK:
            if (button_menu_pressed) {
                current_page = MENU_PAGE::IP;
                current_selection = 0;
                break;
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
                break;
            }
            break;

        case MENU_PAGE::IP:
            if (button_menu_pressed) {
                current_page = MENU_PAGE::DHCP;
                current_selection = 0;
                break;
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::NETWORK;
                current_selection = 0;
                break;
            }
            break;

        case MENU_PAGE::DHCP:
            current_selection = 0;
            if (button_down_pressed) {
                if (current_selection < 1) {
                    current_selection++;
                } else {
                    current_selection = 0;
                } 
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 1;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        // enable dhcp
                        break;
                    case 1:
                        current_page = MENU_PAGE::DHCP_STATIC;
                        current_selection = 0;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::IP;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::DHCP_STATIC:
            if (button_down_pressed) {
                if (current_selection < 1) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 1;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        // set ip
                        break;
                    case 1:
                        // set subnet
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::DHCP;
                current_selection = 0;
                break;
            }
                    
            break;

        case MENU_PAGE::PORTS:
            if (button_down_pressed) {
                if (current_selection < 1) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 1;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        current_page = MENU_PAGE::A;
                        current_selection = 0;
                        break;
                    case 1:
                        current_page = MENU_PAGE::B;
                        current_selection = 0;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::A:
            if (button_down_pressed) {
                if (current_selection < 2) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 2;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        current_page = MENU_PAGE::A_STATUS;
                        current_selection = 0;
                        break;
                    case 1:
                        current_page = MENU_PAGE::A_DIRECTION;
                        current_selection = 0;
                        break;
                    case 2:
                        current_page = MENU_PAGE::A_UNIVERSE;
                        current_selection = 0;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::PORTS;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::A_STATUS:
            if (button_menu_pressed) {
                // enable/disable port A
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::A;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::A_DIRECTION:
            if (button_menu_pressed) {
                // set port A direction
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::A;
                current_selection = 0;
            }
            break;
            
        case MENU_PAGE::A_UNIVERSE:
            if (button_down_pressed) {
                if (current_selection < 2) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 2;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        // set net
                        break;
                    case 1:
                        // set subnet
                        break;
                    case 2:
                        // set universe
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::A;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::B:
            if (button_down_pressed) {
                if (current_selection < 2) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 2;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        current_page = MENU_PAGE::B_STATUS;
                        current_selection = 0;
                        break;
                    case 1:
                        current_page = MENU_PAGE::B_DIRECTION;
                        current_selection = 0;
                        break;
                    case 2:
                        current_page = MENU_PAGE::B_UNIVERSE;
                        current_selection = 0;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::PORTS;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::B_STATUS:
            if (button_menu_pressed) {
                // enable/disable port B
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::B;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::B_DIRECTION:
            if (button_menu_pressed) {
                // set port B direction
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::B;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::B_UNIVERSE:
            if (button_down_pressed) {
                if (current_selection < 2) {
                    current_selection++;
                } else {
                    current_selection = 0;
                }
            } else if (button_up_pressed) {
                if (current_selection > 0) {
                    current_selection--;
                } else {
                    current_selection = 2;
                }
            } else if (button_menu_pressed) {
                switch (current_selection) {
                    case 0:
                        // set net
                        break;
                    case 1:
                        // set subnet
                        break;
                    case 2:
                        // set universe
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::B;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::STATUS:
            if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
            }
            break;

        case MENU_PAGE::LOCK:
            if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
            }
            break;

    }
    } 
   
};