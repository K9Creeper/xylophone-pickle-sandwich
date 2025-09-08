/// -------------------
/// @file window-list.h

#ifndef WINDOW_LIST_H
#define WINDOW_LIST_H

#include <kernel/data-structures/window-server/window.h>

typedef struct window_list_s{
    window_t** data;
    uint16_t size;
    uint16_t capacity;
}window_list_t;

#endif