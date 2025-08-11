/// ----------
/// graphics.h
/// @brief This file contains the main graphics interface for the project.

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <data-structures/graphics/framebuffer.h>

extern void graphics_init(uint32_t lfb_location, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp);
extern void graphics_fill_screen(uint32_t color);
extern void graphics_paint(int x, int y, uint32_t color, uint8_t opacity);
extern void graphics_paint_rect(int x, int y, int w, int h, uint32_t color, uint8_t opacity);

// backbuffer
extern framebuffer_t* graphics_get_buffer(void);

extern void graphics_swap_buffers(bool clear_after_swap);

#endif