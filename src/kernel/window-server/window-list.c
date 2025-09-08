/// -------------------
/// @file window-list.c

#include "kernel/window-server/window-list.h"

#include <stddef.h>

static inline void swap(window_t **a, window_t **b)
{
    window_t *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(window_list_t *list, uint32_t idx)
{
    while (idx > 0)
    {
        uint32_t parent = (idx - 1) / 2;
        if (list->data[parent]->z_index >= list->data[idx]->z_index)
            break;
        swap(&list->data[parent], &list->data[idx]);
        idx = parent;
    }
}

static void heapify_down(window_list_t *list, uint32_t idx)
{
    while (1)
    {
        uint32_t left = idx * 2 + 1;
        uint32_t right = idx * 2 + 2;
        uint32_t largest = idx;

        if (left < list->size && list->data[left]->z_index > list->data[largest]->z_index)
            largest = left;
        if (right < list->size && list->data[right]->z_index > list->data[largest]->z_index)
            largest = right;

        if (largest == idx)
            break;

        swap(&list->data[idx], &list->data[largest]);
        idx = largest;
    }
}

void window_list_init(window_list_t *list, void *data, uint32_t capacity)
{
    list->data = (window_t **)data;
    list->size = 0;
    list->capacity = capacity;
}

void window_list_push(window_list_t *list, window_t *window)
{
    if (list->size >= list->capacity)
        return;
    list->data[list->size] = window;
    heapify_up(list, list->size);
    list->size++;
}

window_t *window_list_peek(window_list_t *list)
{
    if (list->size == 0)
        return NULL;
    return list->data[0]; // max z-index
}

window_t *window_list_pop(window_list_t *list)
{
    if (list->size == 0)
        return NULL;
    window_t *top = list->data[0];
    list->size--;
    if (list->size > 0)
    {
        list->data[0] = list->data[list->size];
        heapify_down(list, 0);
    }
    return top;
}

static void _window_list_raise(window_list_t *list, uint32_t idx)
{
    if (idx >= list->size)
        return;

    uint32_t max_z = list->data[0]->z_index;
    list->data[idx]->z_index = max_z + 1;

    heapify_up(list, idx);
}

void window_list_raise(window_list_t *list, window_t *win)
{
    for (uint32_t i = 0; i < list->size; i++)
    {
        if (list->data[i] == win)
        {
            _window_list_raise(list, i);
            break;
        }
    }
}