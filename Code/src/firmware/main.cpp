#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

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

#define FONT font_12x16 // TODO: change to smaller font. (create?)

enum MENU_PAGE {
    MAIN,
    NETWORK,
    IP,
    DHCP, 
//    DHCP_STATIC,
    PORTS,
    A,
    A_STATUS,
    A_UNIVERSE,
    B,
    B_STATUS,
    B_UNIVERSE,
    STATUS,
    LOCK            // Lock/unlock the menu by holding the menu button for 3 seconds.
};

enum PORT_STATUS {
    INPUT,
    OUTPUT,
    DISABLED
};

using ConfigTypes = std::variant<bool, std::string, uint16_t, PORT_STATUS>;

std::unordered_map<std::string, ConfigTypes> config = {
    {"IP", "002.000.000.001"},
    {"SUBNET", "255.000.000.000"},
    {"DHCP", true},
    {"PORT_A_STATUS", PORT_STATUS::OUTPUT},
    {"PORT_A_UNIVERSE", uint16_t(0x0000)},
    {"PORT_B_STATUS", PORT_STATUS::OUTPUT},
    {"PORT_B_UNIVERSE", uint16_t(0x0001)}
};

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
//              disable
//              input             // may support later
//              ouput  
//          - universe
//              net
//              subnet
//              universe
//  - Status ??
//
//





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
    i2c_init(i2c0, 100 * 1000);        // 100 kHz (quicker will make the menu untraversable)
    //i2c_init(i2c0, 3960 * 1000);        // 3.96 MHz. (ssd1306 is max 10Mhz but this is the limit for my module.)

    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);

    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
}

void draw_menu_content(pico_ssd1306::SSD1306 *ssd1306, std::string title, std::vector<std::string> content, bool clear = true) {
    if (clear) {
    ssd1306->clear();
    }
    drawLine(ssd1306, 0, 15, 128, 15);
    // Draw the title
    drawText(ssd1306, FONT, title.c_str(), 0, 0);
    // Draw the menu items
    for (size_t i = 0; i < content.size(); ++i) {
        drawText(ssd1306, FONT, content[i].c_str(), 0, 16 + i * 16);
    }
}

void draw_menu_selected(pico_ssd1306::SSD1306 *ssd1306, uint8_t selected) {
    fillRect(ssd1306, 0, 16 + selected * 16, 128, 31 + selected * 16, WriteMode::INVERT);
}

void draw_menu(pico_ssd1306::SSD1306 *ssd1306, std::string title, std::vector<std::string> content, uint8_t selected, bool sendBuffer = true) {
    draw_menu_content(ssd1306, title, content);
    draw_menu_selected(ssd1306, selected);
    if (sendBuffer) {
        ssd1306->sendBuffer();
    }
}

void switch_menu(MENU_PAGE &current_page, MENU_PAGE new_page, uint8_t &current_selection) {
    current_page = new_page;
    current_selection = 0;
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
    

    // ---------

    
    MENU_PAGE current_page = MENU_PAGE::MAIN;
    uint8_t current_selection = 0;

    // for editing ip, subnet, universe, etc.
    // keeps the focus on the current window to allow editing of values.
    bool edit_mode = false;
    
    while (true) {
    // Invert pin values because they are active low.
    // TODO: implement debouncing
    bool button_menu_pressed = !gpio_get(MENU_BUTTON_PIN);
    bool button_up_pressed = !gpio_get(UP_BUTTON_PIN);
    bool button_down_pressed = !gpio_get(DOWN_BUTTON_PIN);
    bool button_exit_pressed = !gpio_get(EXIT_BUTTON_PIN);

    // TODO: refactor the whole menu system. it's still in development but a mess rn.
    // - move the display "printing" to the top of the case.
    // - reorder the button checks to the order: exit, menu, down, up.
    // TODO: switch all menu changes to the menu_switch function.
    // if possible move the debug print to the menu_switch function.
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
                        printf("enter menu network\n");
                        switch_menu(current_page, MENU_PAGE::NETWORK, current_selection);
                        break;
                    case 1:
                        printf("enter menu ports\n");
                        switch_menu(current_page, MENU_PAGE::PORTS, current_selection);
                        break;
                    case 2:
                        printf("enter menu status\n");
                        switch_menu(current_page, MENU_PAGE::STATUS, current_selection);
                        break;
                }
            } else if (button_exit_pressed) {
                current_selection = 0;
            }
            draw_menu(&display, "Main:", {"Network", "Ports", "Status"}, current_selection);
            break;

        case MENU_PAGE::NETWORK:
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
                        current_page = MENU_PAGE::IP;
                        current_selection = 0;
                        printf("enter menu ip\n"); 
                        break;
                    case 1:
                        current_page = MENU_PAGE::DHCP;
                        current_selection = 0;
                        printf("enter menu dhcp\n");
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
                printf("enter menu main\n");
            }
            draw_menu(&display, "Network:", {"IP", "DHCP"}, current_selection);
            break;

        case MENU_PAGE::DHCP:
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
                    case 0: // Enable (DHCP)
                        config["DHCP"] = true;
                        break;
                    case 1: // Disable (DHCP)
                        config["DHCP"] = false;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::NETWORK;
                current_selection = 0;
                printf("enter menu network\n");
                break;
            }
            draw_menu_content(&display, "DHCP:", {"Enable", "Disable"});
            drawRect(&display, 112, 16, 127, 31);
            drawRect(&display, 112, 32, 127, 47);
            if (std::get<bool>(config["DHCP"])) {   // dhcp enabled
                drawText(&display, FONT, "v", 114, 15);
            } else {    // dhcp disabled
                drawText(&display, FONT, "x", 114, 31);
            }
            draw_menu_selected(&display, current_selection);
            display.sendBuffer();
            // TODO: add box/tick to selected option
            break;

        case MENU_PAGE::IP:
            display.clear();
            draw_menu_content(&display, "IP:", {}, false);
            drawText(&display, font_8x8, "IP Address:", 0, 16);
            drawText(&display, font_8x8, std::get<std::string>(config["IP"]).c_str(), 0, 24);
            drawText(&display, font_8x8, "Subnet Mask:", 0, 32);
            drawText(&display, font_8x8, std::get<std::string>(config["SUBNET"]).c_str(), 0, 40);

            if (!edit_mode) {
                if (button_exit_pressed) {
                    current_page = MENU_PAGE::NETWORK;
                    current_selection = 0;
                    printf("enter menu network\n");
                    break;
                } else if (button_menu_pressed) {
                    edit_mode = true;
                } else if (button_down_pressed) {
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
                }
            } else { // edit mode enabled
                switch (current_selection) {
                    case 0: // IP
                        break;
                    case 1: // Subnet
                        break;
            }
            }
            draw_menu_selected(&display, current_selection);
            display.sendBuffer();
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
                        printf("enter menu A\n");
                        break;
                    case 1:
                        current_page = MENU_PAGE::B;
                        current_selection = 0;
                        printf("enter menu B\n");
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
                printf("enter menu main\n");
                break;
            }
            draw_menu(&display, "Ports:", {"Port A", "Port B"}, current_selection);
            break;

        case MENU_PAGE::A:
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
                        current_page = MENU_PAGE::A_STATUS;
                        current_selection = 0;
                        printf("enter menu A status\n");
                        break;
                    case 1:
                        current_page = MENU_PAGE::A_UNIVERSE;
                        current_selection = 0;
                        printf("enter menu A universe\n");
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::PORTS;
                current_selection = 0;
                printf("enter menu ports\n");
            }
            draw_menu(&display, "Port A:", {"Status", "Universe"}, current_selection);
            break;

        case MENU_PAGE::A_STATUS:
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
                    case 0: // output`
                        config["PORT_A_STATUS"] = PORT_STATUS::OUTPUT;
                        break;
                    case 1: // input
                        break;
                        // TODO: remove break if input support is added.
                        config["PORT_A_STATUS"] = PORT_STATUS::INPUT;
                        break;
                    case 2: // disabled
                        config["PORT_A_STATUS"] = PORT_STATUS::DISABLED;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::A;
                current_selection = 0;
                printf("enter menu A\n");
                break;
            }
            draw_menu_content(&display, "Status A:", {"Output", "Input", "Disable"});
            drawLine(&display, 0, 40, 128, 40);     // TODO: remove when input is supported.
            drawRect(&display, 112, 16, 127, 31);
            drawRect(&display, 112, 32, 127, 47);
            drawRect(&display, 112, 48, 127, 63);
            switch (std::get<PORT_STATUS>(config["PORT_A_STATUS"])) {
                case PORT_STATUS::OUTPUT:
                    drawText(&display, FONT, "v", 114, 15);
                    break;
                case PORT_STATUS::INPUT:
                    drawText(&display, FONT, "v", 114, 31);
                    break;
                case PORT_STATUS::DISABLED:
                    drawText(&display, FONT, "v", 114, 47);
                    break;
            }
            draw_menu_selected(&display, current_selection);
            display.sendBuffer();
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
                printf("enter menu A\n");
                break;
            }
            // TODO: cast universe to uint16_t when changing
            // TODO: add a way to set the net/subnet/universe. default menu is temporary.
            draw_menu(&display, "Universe A:", {"Net", "Subnet", "Universe"}, current_selection);
            break;

        case MENU_PAGE::B:
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
                        current_page = MENU_PAGE::B_STATUS;
                        current_selection = 0;
                        printf("enter menu B status\n");
                        break;
                    case 1:
                        current_page = MENU_PAGE::B_UNIVERSE;
                        current_selection = 0;
                        printf("enter menu B universe\n");
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::PORTS;
                current_selection = 0;
                printf("enter menu ports\n");
                break;
            }
            draw_menu(&display, "Port B:", {"Status", "Universe"}, current_selection);
            break;

        case MENU_PAGE::B_STATUS:
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
                switch(current_selection) {
                    case 0:
                        config["PORT_B_STATUS"] = PORT_STATUS::OUTPUT;
                        break;
                    case 1:
                        break;
                        // TODO: remove break if input support is added.
                        config["PORT_B_STATUS"] = PORT_STATUS::INPUT;
                        break;
                    case 2:
                        config["PORT_B_STATUS"] = PORT_STATUS::DISABLED;
                        break;
                }
            } else if (button_exit_pressed) {
                current_page = MENU_PAGE::B;
                current_selection = 0;
                printf("enter menu B\n");
                break;
            }
            draw_menu_content(&display, "Status B:", {"Output", "Input", "Disable"});
            drawLine(&display, 0, 40, 128, 40);     // TODO: remove when input is supported.
            drawRect(&display, 112, 16, 127, 31);
            drawRect(&display, 112, 32, 127, 47);
            drawRect(&display, 112, 48, 127, 63);
            switch(std::get<PORT_STATUS>(config["PORT_B_STATUS"])) {
                case PORT_STATUS::OUTPUT:
                    drawText(&display, FONT, "v", 114, 15);
                    break;
                case PORT_STATUS::INPUT:
                    drawText(&display, FONT, "v", 114, 31);
                    break;
                case PORT_STATUS::DISABLED:
                    drawText(&display, FONT, "v", 114, 47);
                    break;
            }
            draw_menu_selected(&display, current_selection);
            display.sendBuffer();
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
                printf("enter menu B\n");
                break;
            }
            // TODO: cast universe to uint16_t when changing
            break;

        case MENU_PAGE::STATUS:
            if (button_exit_pressed) {
                current_page = MENU_PAGE::LOCK;
                current_selection = 0;
                printf("enter menu lock\n");
                break;
            }
            draw_menu_content(&display, "Status:", {});
            drawText(&display, FONT, "Nothing rn", 0, 16);
            display.sendBuffer();
            break;

        case MENU_PAGE::LOCK:
            if (button_exit_pressed) {
                current_page = MENU_PAGE::MAIN;
                current_selection = 0;
                printf("enter menu main\n");
                break;
            }
            display.clear();
            drawText(&display, FONT, "Locked", 24, 24);
            display.sendBuffer();
            break;

    }
    }
   
};