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
extern void graphics_paint_icon(const uint32_t*icon, int x, int y, int size_x, int size_y);
extern void graphics_paint_char(char character, int x, int y, float font_width, float font_height,  uint32_t color, uint8_t opacity, float h_scale, float v_scale, bool flip_x, bool flip_y);
extern void graphics_paint_string(const char *str, int len, int x, int y, float font_width, float font_height,  uint32_t color, uint8_t opacity, float h_scale, float v_scale, int spacing_x, int spacing_y, bool flip_x, bool flip_y);

extern void graphics_paint_framebuffer(framebuffer_t* framebuffer, int x, int y);

// backbuffer
extern framebuffer_t* graphics_get_buffer(void);

extern void graphics_swap_buffers(bool clear_after_swap);

#endif