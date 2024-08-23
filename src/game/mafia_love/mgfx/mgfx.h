#ifndef MGFX_H
#define MGFX_H

#include <stdint.h>
#include "mgfxt.h"

/*
 * Mihir Graphics (gfx)
 *
 * The ST7735 library we have is kinda slow, so
 * I went ahead and made a (hopefully) faster one
 *
 * This is not a general purpose library, it is
 * designed specifically for our game, so you
 * might not be able to use it for your use
 *
 * This uses a color index framebuffer (there
 * is a palette array and the "framebuffer" is
 * indices of the pallete)
 */
#define WIN_W 128
#define WIN_H 160
#define WIN_XMAX (WIN_W - 1)
#define WIN_YMAX (WIN_H - 1)

void mgfx_init       (void);
void mgfx_send       (void);
void mgfx_draw_pixel (uint8_t x, uint8_t y, uint8_t color_index);
void mgfx_draw_rect  (uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color_index);
void mgfx_draw_array (uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *array);
void mgfx_draw_sd    (int16_t x, int16_t y, uint32_t i);

#endif