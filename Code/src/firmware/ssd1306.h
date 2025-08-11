// ssd1306.h
// library for ssd1306 based oled displays.

#ifndef SSD1306_H
#define SSD1306_H


#include "pico/stdlib.h"
#include "hardware/i2c.h"

// (128x64)/8 = 1024 bytes
// for both 128x64 and 128x32 displays due to memory mapping
// in the ssd1306.
#define FRAMEBUFFER_SIZE 1024

void ssd1306_byte_or(ssd1306_display_t *display, uint16_t idx, uint8_t byte);

void ssd1306_byte_and(ssd1306_display_t *display, uint16_t idx, uint8_t byte);

void ssd1306_byte_xor(ssd1306_display_t *display, uint16_t idx, uint8_t byte);

void ssd1306_set_buffer(ssd1306_display_t *display, uint8_t *new_buffer);

const enum register_addresses {
    SSD1306_CONTRAST = 0x81;
    SSD1306_DISPLAYALL_ON_RESUME = 0xA4;
    SSD1306_DISPLAYALL_ON = 0xA5;
    SSD1306_INVERTED_OFF = 0xA6;
    SSD1306_INVERTED_ON = 0xA7;
    SSD1306_DISPLAY_OFF = 0xAE;
    SSD1306_DISPLAY_ON = 0xAF;
    SSD1306_DISPLAYOFFSET = 0xD3;
    SSD1306_COMPINS = 0xDA;
    SSD1306_VCOMDETECT = 0x0B;
    SSD1306_DISPLAYCLOCKDIV = 0xD5;
    SSD1306_PRECHARGE = 0xD9;
    SSD1306_MULTIPLEX = 0xA8;
    SSD1306_LOWCOLUMN = 0x00;
    SSD1306_HIGHCOLUMN = 0x10;
    SSD1306_STARTLINE = 0x40;
    SSD1306_MEMORYMODE = 0x20;
    SSD1306_MEMORYMODE_HORIZONTAL = 0x00;
    SSD1306_MEMORYMODE_VERTICAL = 0x01;
    SSD1306_MEMORYMODE_PAGE = 0x10;
    SSD1306_COLUMNADDR = 0x21;
    SSD1306_COM_REMAP_OFF = 0xC0;
    SSD1306_COM_REMAP_ON = 0xC8;
    SSD1306_CLUMN_REMAP_OFF = 0xA0;
    SSD1306_CLUMN_REMAP_ON = 0xA1;
    SSD1306_CHARGEPUMP = 0x8D;
    SSD1306_EXTERNALVCC = 0x1;
    SSD1306_SWITCHCAPVCC = 0x2;
};

typedef struct {
    i2c_inst_t *i2c_inst; 
    uint16_t i2c_addr;
    uint8_t *buffer;
    uint8_t width;  // 128px
    uint8_t height; // 64|32px
} ssd1306_display_t;

typedef enum write_mode_t {
    ADD = 0,
    SUBTRACT = 1,
    INVERT = 2,
};

void ssd1306_send_command(ssd1306_display_t *display, uint8_t command);

void ssd1306_set_pixel(ssd1306_display_t *display, uint8_t x, uint8_t y, write_mode_t mode = ADD);

void ssd1306_send_buffer(ssd1306_display_t *display);

void ssd1306_add_bitmap_image(ssd1306_display_t *display, uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                              uint8_t *image, write_mode_t mode = ADD);

void ssd1306_clear(ssd1306_display_t *display);

void ssd1306_set_contrast(ssd1306_display_t *display, uint8_t contrast);

void ssd1306_turn_on(ssd1306_display_t *display);

void ssd1306_turn_off(ssd1306_display_t *display);




#endif // SSD1306_H
