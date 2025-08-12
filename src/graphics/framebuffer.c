/// -------------
/// framebuffer.c
/// @brief This file defines the framebuffer functions, for handling the system display.

#include <graphics/framebuffer.h>

#include <stddef.h>
#include <memory.h>

void framebuffer_init(framebuffer_t *fb, uint32_t *lfb, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp)
{
    if (fb == NULL || lfb == NULL)
        return;

    fb->lfb = lfb;
    fb->width = width;
    fb->height = height;
    fb->pitch = pitch;
    fb->bpp = bpp;

    fb->size = ((uint32_t)(lfb) + ((height - 1) * pitch + ((width - 1) * (bpp / 8)))) - (uint32_t)(lfb);

    framebuffer_clear(fb);
}

void framebuffer_clear(framebuffer_t* fb){
    if (fb == NULL || fb->lfb == NULL)
        return;

    fast_memset(fb->lfb, 0, fb->size);
}

uint32_t* framebuffer_get_pixel(framebuffer_t *fb, int x, int y)
{
    if (!fb || !fb->lfb)
        return NULL;

    if (x < 0 || y < 0 || x >= fb->width || y >= fb->height)
        return NULL;

    const uint32_t bytes_per_pixel = fb->bpp / 8;
    return (uint32_t*)((uint8_t*)fb->lfb + y * fb->pitch + x * bytes_per_pixel);
}