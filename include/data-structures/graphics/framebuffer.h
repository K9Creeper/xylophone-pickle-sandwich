/// -------------
/// framebuffer.h
/// @brief This defines the framebuffer structure, for handling the system display.

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

typedef struct framebuffer_s
{
    uint32_t *lfb;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;

    uint32_t size;

    uint32_t used_width;
    uint32_t used_height;
} framebuffer_t;

#endif // FRAMEBUFFER_H