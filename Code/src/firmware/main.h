
// Config made for a RP2040 Zero. (Pi pico in smaller form factor)
//      
//               ┏━━━━━┓              
//           ┏━━━┫     ┣━━━┓          
//        5V ┃◎  ┗━━━━━┛  ◎┃ GP0
//       Gnd ┃▣           ◎┃ GP1
//       3V3 ┃◎           ◎┃ GP2
//      GP29 ┃◎           ◎┃ GP3
//      GP28 ┃◎           ◎┃ GP4
//      GP27 ┃◎     ▩     ◎┃ GP5
//      GP26 ┃◎     ┗GP16 ◎┃ GP6
//      GP15 ┃◎           ◎┃ GP7
//      GP14 ┃◎ ◎ ◎ ◎ ◎ ◎ ◎┃ GP8
//           ┗━━━━━━━━━━━━━┛
//              ┃ ┃ ┃ ┃ ┗ GP9
//              ┃ ┃ ┃ ┗ GP10
//              ┃ ┃ ┗ GP11
//              ┃ ┗ GP 12
//              ┗ GP13

#ifndef MAIN_H
#define MAIN_H

// General
#define UNIVERSE_LENGTH 512

// DMX Ports
#define PORT_A_ENABLE   1
#define PORT_A_TX_PIN   3
#define PORT_A_RX_PIN   0
#define PORT_A_DIR_PIN  1
#define PORT_A_LED_PIN  2

#define PORT_B_ENABLE   1
#define PORT_B_TX_PIN   7
#define PORT_B_RX_PIN   4
#define PORT_B_DIR_PIN  5
#define PORT_B_LED_PIN  6

// Display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64 // or 32. don't know what i will go with yet

#define DIPLAY_SDA_PIN  8
#define DIPLAY_SCL_PIN  9

// Buttons/Controls. Don't know yet if I want to use 4 buttons or a push encoder (+ 1 button?)
#define MENU_BUTTON_PIN 10  // Menu/Enter
#define UP_BUTTON_PIN   11  // Up
#define DOWN_BUTTON_PIN 12  // Down
#define EXIT_BUTTON_PIN 13  // Exit/Back

// #define ENCODER_A_PIN   10
// #define ENCODER_B_PIN   11
// #define ENCODER_SW_PIN  12
// #define BUTTON_PIN      13

// Ethernet (enc28j60)

#define ETH_CS_PIN      29
#define ETH_MOSI_PIN    28
#define ETH_MISO_PIN    27
#define ETH_SCK_PIN     26
#define ETH_INT_PIN     15
#define ETH_RST_PIN     14



#endif // MAIN_H