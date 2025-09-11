/// -------------------
/// @file window-list.h

#ifndef WINDOW_LIST_H
#define WINDOW_LIST_H

#include <kernel/data-structures/window-server/window.h>

typedef struct window_list_s{
    window_t* head;
    window_t* tail;

    window_t** data;

    int size;
    int capacity;
}window_list_t;

#endif