/// --------
/// window.h
/// @brief This file defines the graphical window structure.

#ifndef WINDOW_H
#define WINDOW_H

#include <data-structures/graphics/framebuffer.h>

typedef struct window_s{
    char title[64];
    int x, y;
    int width, height;
    framebuffer_t fb;
}window_t;

#endif