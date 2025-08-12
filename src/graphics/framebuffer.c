/// -------------
/// framebuffer.c
/// @brief This file defines the framebuffer functions, for handling the system display.

#include <graphics/framebuffer.h>

#include <stddef.h>

void framebuffer_init(framebuffer_t *fb, uint32_t *lfb, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp)
{
    if (fb == NULL || lfb == NULL)
        return;

    fb->lfb = lfb;
    fb->width = width;
    fb->height = height;
    fb->pitch = pitch;
    fb->bpp = bpp;
    fb->bytes_per_pixel = bpp / 8;
    fb->size = fb->width * fb->height * fb->bytes_per_pixel;

    framebuffer_clear(fb);
}