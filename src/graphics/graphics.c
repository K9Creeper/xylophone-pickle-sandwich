/// ----------
/// graphics.c
/// @brief This file defines the main graphics interface for the project.

#include <graphics/graphics.h>
#include <graphics/framebuffer.h>

#include <stddef.h>
#include <memory.h>

#include <kernel/stdlib.h>

#include <graphics/fonts.h>

#include <math.h>

static framebuffer_t foreground_buffer;
static framebuffer_t background_buffer;

void graphics_init(uint32_t lfb_location, uint32_t width, uint32_t height, uint32_t pitch, uint32_t bpp)
{
    framebuffer_init(&foreground_buffer, (uint32_t *)lfb_location, width, height, pitch, bpp);
    framebuffer_init(&background_buffer, (uint32_t *)kernel_calloc(1, foreground_buffer.size), width, height, pitch, bpp);
}

void graphics_fill_screen(uint32_t color)
{
    if (background_buffer.lfb == NULL)
        return;

    uint32_t *pixels = background_buffer.lfb;
    uint32_t count = background_buffer.width * background_buffer.height;
    for (uint32_t i = 0; i < count; i++)
    {
        pixels[i] = color;
    }
}

static inline uint8_t blend_channel(uint8_t src, uint8_t dst, uint8_t alpha)
{
    // alpha: 0-255, blend src over dst
    return (uint8_t)((src * alpha + dst * (255 - alpha)) / 255);
}

void graphics_paint(int x, int y, uint32_t color, uint8_t opacity)
{
    if (!background_buffer.lfb || opacity == 0)
        return;

    if (x < 0 || y < 0 || x >= (int)background_buffer.width || y >= (int)background_buffer.height)
        return;

    uint32_t *pixel = framebuffer_get_pixel(&background_buffer, x, y);
    if (!pixel)
        return;

    if (opacity == 255)
    {
        *pixel = color;
    }
    else
    {
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

void graphics_paint_rect(int x, int y, int w, int h, uint32_t color, uint8_t opacity)
{
    if (!background_buffer.lfb || opacity == 0)
        return;

    if (x < 0)
    {
        w += x;
        x = 0;
    }
    if (y < 0)
    {
        h += y;
        y = 0;
    }
    if (x + w > (int)background_buffer.width)
        w = background_buffer.width - x;
    if (y + h > (int)background_buffer.height)
        h = background_buffer.height - y;

    if (w <= 0 || h <= 0)
        return;

    for (int j = 0; j < h; j++)
    {
        uint32_t *row = framebuffer_get_pixel(&background_buffer, x, y + j);
        if (!row)
            continue;

        if (opacity == 255)
        {
            for (int i = 0; i < w; i++)
            {
                row[i] = color;
            }
        }
        else
        {
            for (int i = 0; i < w; i++)
            {
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

void graphics_paint_icon(const uint32_t *icon, int x, int y, int size_x, int size_y)
{
    for (int j = 0; j < size_y; j++)
    {
        if (y + j > background_buffer.height)
            break;
        for (int i = 0; i < size_x; i++)
        {
            if (x + i > background_buffer.width)
                break;

            uint32_t src = icon[j * size_x + i];
            if (src == 0x0 /* 0x0 == transparent */)
                continue;
            uint32_t *dst = framebuffer_get_pixel(&background_buffer, x + i, y + j);
            if (dst == NULL)
                continue;
            *dst = src;
        }
    }
}

void graphics_paint_char(char character, int x, int y, float font_width, float font_height,
                         uint32_t color, uint8_t opacity, float h_scale, float v_scale,
                         bool flip_x, bool flip_y)
{
    if (!background_buffer.lfb || opacity == 0)
        return;

    if ((unsigned char)character >= 128)
        return;

    float scale_x = (font_width / 8.0f) * h_scale;
    float scale_y = (font_height / 8.0f) * v_scale;

    for (int row = 0; row < 8; row++)
    {
        uint8_t bits = font8x8_basic[(uint8_t)character][row];
        for (int col = 0; col < 8; col++)
        {
            if (bits & (1 << col))
            {
                // Flipping
                int draw_col = flip_x ? (7 - col) : col;
                int draw_row = flip_y ? (7 - row) : row;

                int px = x + (int)(draw_col * scale_x);
                int py = y + (int)(draw_row * scale_y);

                int block_w = (int)ceilf(scale_x);
                int block_h = (int)ceilf(scale_y);

                for (int dy = 0; dy < block_h; dy++)
                {
                    int drawY = py + dy;
                    if (drawY < 0 || drawY >= background_buffer.height)
                        continue;

                    for (int dx = 0; dx < block_w; dx++)
                    {
                        int drawX = px + dx;
                        if (drawX < 0 || drawX >= background_buffer.width)
                            continue;

                        uint32_t *pixel = framebuffer_get_pixel(&background_buffer, drawX, drawY);
                        if (!pixel)
                            continue;

                        if (opacity == 255)
                        {
                            *pixel = color;
                        }
                        else
                        {
                            uint8_t srcR = (color >> 16) & 0xFF;
                            uint8_t srcG = (color >> 8) & 0xFF;
                            uint8_t srcB = color & 0xFF;

                            uint8_t dstR = (*pixel >> 16) & 0xFF;
                            uint8_t dstG = (*pixel >> 8) & 0xFF;
                            uint8_t dstB = *pixel & 0xFF;

                            *pixel = (blend_channel(srcR, dstR, opacity) << 16) |
                                     (blend_channel(srcG, dstG, opacity) << 8) |
                                     blend_channel(srcB, dstB, opacity);
                        }
                    }
                }
            }
        }
    }
}

void graphics_paint_string(const char *str, int len, int x, int y, float font_width, float font_height,
                           uint32_t color, uint8_t opacity, float h_scale, float v_scale,
                           int spacing_x, int spacing_y, bool flip_x, bool flip_y)
{
    if (!str || len <= 0)
        return;

    for (int i = 0; i < len; i++)
    {
        graphics_paint_char(str[i],
                            x + i * ((int)ceilf(font_width * h_scale) + spacing_x),
                            y,
                            font_width, font_height, color, opacity,
                            h_scale, v_scale,
                            flip_x, flip_y);
    }
}

void graphics_paint_framebuffer(framebuffer_t *fb, int x, int y)
{
    if (!fb || !fb->lfb)
        return;

    int draw_width = fb->width;
    int draw_height = fb->height;

    for (int j = 0; j < draw_height; j++)
    {
        for (int i = 0; i < draw_width; i++)
        {
            uint32_t *dst = framebuffer_get_pixel(&background_buffer, x + i, y + j);
            uint32_t *src = framebuffer_get_pixel(fb, x, j);
            if (!dst || !src)
                continue;
            *dst = *src;
        }
    }
}

framebuffer_t *graphics_get_buffer(void)
{
    return &background_buffer;
}

void graphics_swap_buffers(bool clear_after_swap)
{
    if (foreground_buffer.lfb == NULL || background_buffer.lfb == NULL)
        return;

    if (foreground_buffer.size != background_buffer.size)
        return;

    fast_memcpy((foreground_buffer.lfb), (background_buffer.lfb), foreground_buffer.size);

    if (clear_after_swap)
        framebuffer_clear(&background_buffer);
}