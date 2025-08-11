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

    for (uint32_t y = 0; y < background_buffer.height; y++)
    {
        for (uint32_t x = 0; x < background_buffer.width; x++)
        {
            // NOTE: Could just call the graphics_paint function here, but this is faster (i think)
            uint32_t* pixel = framebuffer_get_pixel(&background_buffer, x, y);
            if (pixel)
                *pixel = color;
        }
    }
}

void graphics_paint(int x, int y, uint32_t color, uint8_t opacity){
    uint32_t* pixel = framebuffer_get_pixel(&background_buffer, x, y);
    if (pixel) {
        if (opacity == 255) {
            (*pixel) = color;
        } else {
            uint8_t srcR = (color >> 16) & 0xFF;
            uint8_t srcG = (color >> 8) & 0xFF;
            uint8_t srcB = color & 0xFF;
            
            uint8_t dstR = (*pixel >> 16) & 0xFF;
            uint8_t dstG = (*pixel >> 8) & 0xFF;
            uint8_t dstB = (*pixel) & 0xFF;

            uint8_t outR = ((srcR * opacity) + (dstR * (255 - opacity))) / 255;
            uint8_t outG = ((srcG * opacity) + (dstG * (255 - opacity))) / 255;
            uint8_t outB = ((srcB * opacity) + (dstB * (255 - opacity))) / 255;

            *pixel = (outR << 16) | (outG << 8) | outB;
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