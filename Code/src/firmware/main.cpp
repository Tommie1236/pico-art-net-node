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

    uint32_t mask = 0;
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



    // ---------

    
    MENU_PAGE current_page = MAIN;
    uint8_t current_selection = 0;
    bool button_menu_pressed = false;
    bool button_up_pressed = false;
    bool button_down_pressed = false;
    bool button_exit_pressed = false;

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
    
   
};