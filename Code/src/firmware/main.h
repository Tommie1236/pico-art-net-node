
// Config file for Pico Art-Net DMX Node
// ©Timo Oosterom 2024

// Config made for a RP2040 Zero. (Pi pico in smaller form factor)
// For full pin functions see: https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf (Section 1.4.3)
//      
//                   ┏━━━━━┓              
//               ┏━━━┫     ┣━━━┓          
//            5V ┃◎  ┗━━━━━┛  ◎┃ GP0 PORT_A_RX    
//           Gnd ┃■           ◎┃ GP1 PORT_A_DIR    
//           3V3 ┃◎           ◎┃ GP2 PORT_A_LED    
// ETH_CS   GP29 ┃◎           ◎┃ GP3 PORT_A_TX    
// ETH_MOSI GP28 ┃◎           ◎┃ GP4 PORT_B_RX    
// ETH_MISO GP27 ┃◎     □     ◎┃ GP5 PORT_B_DIR    
// ETH_SCK  GP26 ┃◎     ┗GP16 ◎┃ GP6 PORT_B_LED    
// ETH_INT  GP15 ┃◎           ◎┃ GP7 PORT_B_TX    
// ETH_RST  GP14 ┃◎ ◎ ◎ ◎ ◎ ◎ ◎┃ GP8 DISPLAY_SDA    
//               ┗━━━━━━━━━━━━━┛
//                  ┃ ┃ ┃ ┃ ┗ GP9    DISPLAY_SCL    
//                  ┃ ┃ ┃ ┗ GP10     MENU_BUTTON    
//                  ┃ ┃ ┗ GP11       UP_BUTTON
//                  ┃ ┗ GP12         DOWN_BUTTON
//                  ┗ GP13           EXIT_BUTTON
//
//  GP17-25 Are available via small pads on the bottom side of the board.

#ifndef MAIN_H
#define MAIN_H

// TODO: add default values for settings that can be changed from the menu

// General
#define UNIVERSE_LENGTH 512

#define PORT_A          0
#define PORT_B          1

// DMX Ports
#define PORT_A_ENABLE   1
#define PORT_A_TX_PIN   7
#define PORT_A_RX_PIN   4
#define PORT_A_DIR_PIN  5
#define PORT_A_LED_PIN  6

#define PORT_B_ENABLE   1
#define PORT_B_TX_PIN   3
#define PORT_B_RX_PIN   0
#define PORT_B_DIR_PIN  1
#define PORT_B_LED_PIN  2

// Display
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  32 // 64 or 32. don't know what i will go with yet

#define DISPLAY_SDA_PIN  8
#define DISPLAY_SCL_PIN  9

// Buttons/Controls. 
#define MENU_BUTTON_PIN 10  // Menu/Enter
#define UP_BUTTON_PIN   11  // Up
#define DOWN_BUTTON_PIN 12  // Down
#define EXIT_BUTTON_PIN 13  // Exit/Back

// Ethernet (enc28j60)

#define ETH_CS_PIN      29
#define ETH_MOSI_PIN    28
#define ETH_MISO_PIN    27
#define ETH_SCK_PIN     26
#define ETH_INT_PIN     15
#define ETH_RST_PIN     14
#define ETH_SPI         spi1



#endif // MAIN_H
