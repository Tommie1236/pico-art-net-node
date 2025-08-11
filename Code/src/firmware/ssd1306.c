// ssd1306.c
// library for ssd1306 based oled display

#include "ssd1306.h"


void framebuffer_byte_or(
    uint8_t *buffer, 
    uint16_t idx,
    uint8_t byte) {

    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    buffer[idx] |= byte;
}

void framebuffer_byte_and(
    uint8_t *buffer, 
    uint16_t idx,
    uint8_t byte) {

    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    buffer[idx] &= byte;
}

void framebuffer_byte_xor(
    uint8_t *buffer, 
    uint16_t idx,
    uint8_t byte) {
    
    if (idx > (FRAMEBUFFER_SIZE - 1)) return;
    buffer[idx] ^= byte;
}

