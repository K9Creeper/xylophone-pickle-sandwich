/// -------------------
/// @file window-list.h

#ifndef _WINDOW_LIST_H
#define _WINDOW_LIST_H

#include <kernel/data-structures/window-server/window-list.h>

extern void window_list_init(window_list_t* list, window_t** window_data, int* free_ids, int capacity);

extern uint8_t window_list_add(window_list_t* list, window_t* window);
extern uint8_t window_list_remove(window_list_t* list, window_t* window);

extern uint8_t window_list_raise(window_list_t* list, window_t* window);

#endif