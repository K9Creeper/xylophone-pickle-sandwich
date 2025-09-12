/// -------------------
/// @file window-list.h

#ifndef WINDOW_LIST_H
#define WINDOW_LIST_H

#include <kernel/data-structures/window-server/window.h>

typedef struct window_list_s {
    window_t** data;
    int size;
    int capacity;
    int* free_ids;
    int free_count;
} window_list_t;

#endif