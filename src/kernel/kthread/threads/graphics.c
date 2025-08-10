/// ----------
/// graphics.c
/// @brief This file will handle the graphics renderinng in the system.

#include <graphics/framebuffer.h>
#include <graphics/graphics.h>

#include <data-structures/kernel/kernel-context.h>

#include <stdio.h>
#include <syscall.h>

// kernel-main.c
extern kernel_context_t *kernel_context;

static uint32_t hsv_to_rgb(float h, float s, float v)
{
    int i = (int)(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r, g, b;

    switch (i % 6)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
    default:
        r = g = b = 0;
    }

    uint8_t R = (uint8_t)(r * 255);
    uint8_t G = (uint8_t)(g * 255);
    uint8_t B = (uint8_t)(b * 255);

    return (R << 16) | (G << 8) | B;
}

static void graphics_paint_rect(int x, int y, int width, int height, uint32_t color, uint8_t alpha) {
    for (int py = 0; py < height; py++) {
        for (int px = 0; px < width; px++) {
            int draw_x = x + px;
            int draw_y = y + py;
            if (draw_x >= 0 && draw_x < kernel_context->video_state.width &&
                draw_y >= 0 && draw_y < kernel_context->video_state.height) {
                graphics_paint(draw_x, draw_y, color, alpha);
            }
        }
    }
}


void kthread_graphics(void)
{
    graphics_init(kernel_context->video_state.lfb, kernel_context->video_state.width, kernel_context->video_state.height, kernel_context->video_state.pitch, kernel_context->video_state.bpp);
    printf("Init Graphics 0x%X\n", kernel_context->video_state.lfb);

    int size = 10;
    
    int x = 0;
    int y = kernel_context->video_state.height / 2;
    int dx = 1;
    int dy = 1;

    float hue = 0.0f;

    printf("Rendering...\n");
    while (true)
    {
        // NOTE: ALREADY SETTING TO BLACK BACKGROUND
        // graphics_fill_screen(0xFFFFFF);

        x += dx;
        y += dy;

        // Bounce horizontally with bounds accounting for square size
        if (x <= 0 || x >= kernel_context->video_state.width - size) {
            dx = -dx;
        }

        // Bounce vertically
        if (y <= 0 || y >= kernel_context->video_state.height - size) {
            dy = -dy;
        }

        hue += 0.005f;
        if (hue > 1.0f) hue -= 1.0f;

        uint32_t color = hsv_to_rgb(hue, 1.0f, 1.0f);

        graphics_paint_rect(x, y, size, size, color, 255);

        graphics_swap_buffers(true);
    }
}