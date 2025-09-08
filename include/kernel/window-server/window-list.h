/// -------------------
/// @file window-list.h

#ifndef _WINDOW_LIST_H
#define _WINDOW_LIST_H

#include <kernel/data-structures/window-server/window.h>
#include <kernel/data-structures/window-server/window-list.h>

extern void window_list_init(window_list_t* list, void* data, uint32_t capacity);

extern void window_list_push(window_list_t* list, window_t* window);
extern window_t* window_list_peek(window_list_t* list);
extern window_t* swindow_list_pop(window_list_t* list);

extern void window_list_raise(window_list_t *list, window_t *window);

#endif