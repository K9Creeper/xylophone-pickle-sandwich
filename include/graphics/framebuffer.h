/// -------------
/// framebuffer.h
/// @brief This declares the framebuffer functions, for handling the system display.

#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <data-structures/graphics/framebuffer.h>
#include <memory.h>

extern void framebuffer_init(framebuffer_t *fb, uint32_t *lfb, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp);

static inline void framebuffer_clear(framebuffer_t* fb){
    fast_memset(fb->lfb, 0, fb->size);
}

static inline uint32_t* framebuffer_get_pixel(framebuffer_t *fb, int x, int y) {
    return (uint32_t*)((uint8_t*)fb->lfb + y * fb->pitch + x * fb->bytes_per_pixel);
}

#endif