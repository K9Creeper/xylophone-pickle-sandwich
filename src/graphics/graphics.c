/// ----------
/// graphics.c
/// @brief This file defines the main graphics interface for the project.

#include <graphics/graphics.h>
#include <graphics/framebuffer.h>

#include <stddef.h>
#include <memory.h>

#include <kernel/stdlib.h>

static framebuffer_t foreground_buffer;
static framebuffer_t background_buffer;

void graphics_init(uint32_t lfb_location, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp){
    framebuffer_init(&foreground_buffer, (uint32_t*)lfb_location, width, height, pitch, bpp);
    framebuffer_init(&background_buffer, (uint32_t*)kernel_calloc(1, foreground_buffer.size), width, height, pitch, bpp);
}

void graphics_fill_screen(uint32_t color){
    if (background_buffer.lfb == NULL)
        return;

    uint32_t* pixels = background_buffer.lfb;
    uint32_t count = background_buffer.width * background_buffer.height;
    for (uint32_t i = 0; i < count; i++) {
        pixels[i] = color;
    }
}

static inline uint8_t blend_channel(uint8_t src, uint8_t dst, uint8_t alpha) {
    // alpha: 0-255, blend src over dst
    return (uint8_t)((src * alpha + dst * (255 - alpha)) / 255);
}

void graphics_paint(int x, int y, uint32_t color, uint8_t opacity) {
    if (!background_buffer.lfb || opacity == 0)
        return;

    if (x < 0 || y < 0 || x >= (int)background_buffer.width || y >= (int)background_buffer.height)
        return;

    uint32_t *pixel = framebuffer_get_pixel(&background_buffer, x, y);
    if (!pixel)
        return;

    if (opacity == 255) {
        *pixel = color;
    } else {
        uint8_t srcR = (color >> 16) & 0xFF;
        uint8_t srcG = (color >> 8) & 0xFF;
        uint8_t srcB = color & 0xFF;

        uint8_t dstR = (*pixel >> 16) & 0xFF;
        uint8_t dstG = (*pixel >> 8) & 0xFF;
        uint8_t dstB = *pixel & 0xFF;

        uint8_t outR = blend_channel(srcR, dstR, opacity);
        uint8_t outG = blend_channel(srcG, dstG, opacity);
        uint8_t outB = blend_channel(srcB, dstB, opacity);

        *pixel = (outR << 16) | (outG << 8) | outB;
    }
}

void graphics_paint_rect(int x, int y, int w, int h, uint32_t color, uint8_t opacity) {
    if (!background_buffer.lfb || opacity == 0)
        return;

    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > (int)background_buffer.width) w = background_buffer.width - x;
    if (y + h > (int)background_buffer.height) h = background_buffer.height - y;

    if (w <= 0 || h <= 0)
        return;

    for (int j = 0; j < h; j++) {
        uint32_t *row = framebuffer_get_pixel(&background_buffer, x, y + j);
        if (!row) continue;

        if (opacity == 255) {
            for (int i = 0; i < w; i++) {
                row[i] = color;
            }
        } else {
            for (int i = 0; i < w; i++) {
                uint32_t dst = row[i];

                uint8_t srcR = (color >> 16) & 0xFF;
                uint8_t srcG = (color >> 8) & 0xFF;
                uint8_t srcB = color & 0xFF;

                uint8_t dstR = (dst >> 16) & 0xFF;
                uint8_t dstG = (dst >> 8) & 0xFF;
                uint8_t dstB = dst & 0xFF;

                uint8_t outR = blend_channel(srcR, dstR, opacity);
                uint8_t outG = blend_channel(srcG, dstG, opacity);
                uint8_t outB = blend_channel(srcB, dstB, opacity);

                row[i] = (outR << 16) | (outG << 8) | outB;
            }
        }
    }
}

framebuffer_t* graphics_get_buffer(void){
    return &background_buffer;
}

void graphics_swap_buffers(bool clear_after_swap){
    if(foreground_buffer.lfb == NULL || background_buffer.lfb == NULL)
        return;

    if(foreground_buffer.size != background_buffer.size)
        return;

    fast_memcpy((foreground_buffer.lfb), (background_buffer.lfb), foreground_buffer.size);
    
    if(clear_after_swap)
        framebuffer_clear(&background_buffer);
}