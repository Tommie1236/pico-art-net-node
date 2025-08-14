// ssd1306.c
// library for ssd1306 based oled display

#include "math.h"

#include "ssd1306.h"


void ssd1306_byte_or(
    ssd1306_display_t *display,
    uint16_t idx,
    uint8_t byte) {

    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    display->buffer[idx] |= byte;
}

void ssd1306_byte_and(
    ssd1306_display_t *display,
    uint16_t idx,
    uint8_t byte) {

    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    display->buffer[idx] &= byte;
}

void ssd1306_byte_xor(
    ssd1306_display_t *display,
    uint16_t idx,
    uint8_t byte) {
    
    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    display->buffer[idx] ^= byte;
}

void ssd1306_set_buffer(
    ssd1306_display_t *display,
    uint8_t *new_buffer) {
    
    display->buffer = new_buffer;
}

void ssd1306_send_command(
    ssd1306_display_t *display,
    uint8_t command) {

    uint8_t data[2] = {0x00, command};
    i2c_write_blocking(display->i2c_inst, display->i2c_addr, data, 2, false);
}

void ssd1306_set_pixel(
    ssd1306_display_t *display,
    uint8_t x,
    uint8_t y,
    write_mode_t mode = ADD) {

    // check for valid position
    if ((x < 0) ||
        (x > display->width) || 
        (y < 0) || 
        (y > display->height)) return;

    // TODO: 128x32 needs something special.
    // can't yet figure out what precisely.
    // but it doesn't work the same way so just return.
    if (display->height != 128) return;
    
    uint8_t byte;

    byte = 1 << (y & 7);

    switch (mode) {
        case ADD:
            ssd1306_byte_or(display->buffer, x + (y / 8) * display->width, byte);        
            break;

        case SUBTRACT:
            ssd1306_byte_and(display->buffer, x + (y / 8) * display->width, ~byte);        
            break;

        case INVERT:
            ssd1306_byte_xor(display->buffer, x + (y / 8) * display->width, byte);        
            break;
    }
}

void ssd1306_send_buffer(ssd1306_display_t *display) {
    
    ssd1306_send_command(SSD1306_PAGEADDR);
    ssd1306_send_command(0x00);
    ssd1306_send_command(0x07);
    ssd1306_send_command(SSD1306_COLUMNADDR);
    ssd1306_send_command(0x00);
    ssd1306_send_command(0x7F);

    uint8_t data[FRAMEBUFFER_SIZE + 1];

    data[0] = SSD1306_STARTLINE;

    memcpy(data + 1, display->buffer, FRAMEBUFFER_SIZE);

    i2c_write_blocking(display->i2c_inst, display->i2c_addr, data, FRAMEBUFFER_SIZE + 1, false);
}


// TODO: Add suppport for images. don't feel like implementing
// this right now when i can't test.

// void ssd1306_add_bitmap_image(
//     ssd1306_display_t *display,
//     uint8_t x,
//     uint8_t y,
//     uint8_t width,
//     uint8_t heigth,
//     uint8_t *image,
//     write_mode_t mode = ADD) {
// 
// 
// }

void ssd1306_clear(ssd1306_display_t *display) {

    memset(display->buffer, 0, FRAMEBUFFER_SIZE);
}

void ssd1306_set_contrast(
    ssd1306_display_t *display,
    uint8_t contrast) {

    ssd1306_send_command(SSD1306_CONTRAST);
    ssd1306_send_command(contrast);
}

void ssd1306_turn_on(ssd1306_display_t *display) {
    ssd1306_send_command(SSD1306_DISPLAY_ON);
}

void ssd1306_turn_off(ssd1306_display_t *display) {
    ssd1306_send_command(SSD1306_DISPLAY_OFF);
}

void ssd1306_draw_line(
    ssd1306_display_t *display,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    write_mode_t mode = ADD) {

    uint8_t dx = fabs(x1 - x0);
    uint8_t dy = fabs(y1 - y0);
    int8_t sx = (x1 >= x0) ? 1 : -1;
    int8_t sy = (y1 >= y0) ? 1 : -1;

    if (dx > dy) { // shallow line
        int16_t p = (dy << 1) - dx;
        uint8_t x = x0;
        uint8_t y = y0;

        for (uint8_t i = 0; i =< dx; i++) {
            ssd1306_set_pixel(display, x, y, mode);
            x += sx;

            p += (dy << 1);
            
            if (p >= 0) {
                y += sy;
                p -= (dx << 1);
            }
        }

    } else { // steep line
        int16_t p = (dx << 1) - dy;
        uint8_t x = x0;
        uint8_t y = y0;

        for (uint8_t i = 0; i =< dy; i++) {
            ssd1306_set_pixel(display, x, y, mode);
            y += yx;

            p += (dx << 1);
            
            if (p >= 0) {
                x += xy;
                p -= (dy << 1);
            }
        }


    }

}

void ssd1306_draw_rect(
    ssd1306_display_t *display,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    write_mode_t mode = ADD) {

    // TODO: 
    // doesn't work with coords in wrong order.

    for (uint8_t x = x0; x <= x1, x++) {
        for (uint8_t y = y0; y <= y1, y++) {
            if ((x == x0) ||
                (x == x1) ||
                (y == y0) ||
                (y == y1)) {

                ssd1306_set_pixel(display, x, y, mode)
            }
        }
    }
}

void ssd1306_fill_rect(
    ssd1306_display_t *display,
    uint8_t x0,
    uint8_t y0,
    uint8_t x1,
    uint8_t y1,
    write_mode_t mode = ADD) {

    for (uint8_t x = x0; x <= x1, x++) {
        for (uint8_t y = y0; y <= y1, y++) {
            
            ssd1306_set_pixel(display, x, y, mode)
        }
    }
}
