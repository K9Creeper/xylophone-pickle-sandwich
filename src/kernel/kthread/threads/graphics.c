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

void kthread_graphics(void)
{
    graphics_init(kernel_context->video_state.lfb, kernel_context->video_state.width, kernel_context->video_state.height, kernel_context->video_state.pitch, kernel_context->video_state.bpp);

    while (true)
    {
        graphics_fill_screen(0xfada94);

        mouse_info_t m = mouse_get_info();

        graphics_paint_icon((const uint32_t *)mouse_cursor, m.x, m.y, 9, 14);

        graphics_swap_buffers(false);
        sleep(16);
    }
}
