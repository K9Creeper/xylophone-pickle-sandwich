/// --------
/// window.c
/// @brief This file defines the functions for handling a graphical window.

#include <graphics/window.h>

#include <stddef.h>
#include <memory.h>
#include <string.h>

#include <syscall.h>

#include <data-structures/kernel/kernel-context.h>
// kernel-main.c
extern kernel_context_t* kernel_context;

void window_init(window_t* window, const char* title, int width, int height){
    if(window == NULL || width < 0 || height < 0 || title == NULL || !kernel_context->video_state.is_text_mode) return;

    window->fb.width = window->width = width;
    window->fb.height = window->height = height;

    window->fb.pitch = kernel_context->video_state.pitch;
    window->fb.bpp = kernel_context->video_state.bpp;

    memcpy((uint8_t*)window->title, title, strlen(title));

    window->fb.size = ((window->fb.height - 1) * window->fb.pitch + ((window->fb.width - 1) * (window->fb.bpp / 8)));

    window->fb.lfb = malloc(window->fb.size);
}