/// ----------
/// graphics.c
/// @brief This file will handle the graphics renderinng in the system.

#include <graphics/framebuffer.h>
#include <graphics/graphics.h>
#include <graphics/icons.h>

#include <data-structures/kernel/kernel-context.h>

#include <stdio.h>
#include <syscall.h>
#include <kernel/util.h>

#include "../../drivers/mouse/mouse.h"

// kernel-main.c
extern kernel_context_t *kernel_context;

void draw_mouse_pointer(int x, int y)
{
    for (int j = 0; j < 16; j++)
    {
        for (int i = 0; i < 16; i++)
        {
            if (mouse_cursor[j][i] != 0x0) {
                graphics_paint((x)+i,  (y)+j, mouse_cursor[j][i], 255);
            } 
        }
    }
}

void kthread_graphics(void)
{
    graphics_init(kernel_context->video_state.lfb, kernel_context->video_state.width, kernel_context->video_state.height, kernel_context->video_state.pitch, kernel_context->video_state.bpp);

    while (true)
    {
        graphics_fill_screen(0xfada94);

        mouse_info_t m = mouse_get_info();

        draw_mouse_pointer(m.x, m.y);

        graphics_swap_buffers(false);
        sleep(16);
    }
}
