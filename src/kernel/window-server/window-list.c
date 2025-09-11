/// -------------------
/// @file window-list.c

#include <kernel/window-server/window-list.h>

#include <stddef.h>

static inline int gen_id(window_list_t* list){
    for(int i = 0; i < list->capacity; i++)
        if(list->data[i] == NULL) return i;
    return -1;
}

static inline void swap(window_list_t**a, window_list_t **b)
{
    window_list_t*tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(window_list_t *list, uint32_t idx)
{
    while (idx > 0)
    {
        uint32_t parent = (idx - 1) / 2;
        if (list->data[parent]->z >= list->data[idx]->z)
            break;

        swap(&list->data[parent], &list->data[idx]);
        list->data[idx]->z = list->capacity - idx;

        idx = parent;
    }
}

static void heapify_down(window_list_t *list, uint32_t idx)
{
    while (1)
    {
        uint32_t left = idx * 2 + 1;
        uint32_t right = idx * 2 + 2;
        uint32_t smallest = idx;

        if (left < list->size &&
            list->data[left]->z > list->data[smallest]->z)
        {
            smallest = left;
        }
        if (right < list->size &&
            list->data[right]->z > list->data[smallest]->z)
        {
            smallest = right;
        }
        if (smallest == idx)
            break;

        swap(&list->data[idx], &list->data[smallest]);
        list->data[idx]->z = list->capacity - idx;
        idx = smallest;
    }
}

void window_list_init(window_list_t* list, window_t** window_data, int capacity){
    list->head = NULL;
    list->tail = NULL;

    list->data = window_data;

    list->size = 0;
    list->capacity = capacity;
}

uint8_t window_list_add(window_list_t* list, window_t* window){
    if(list->size >= list->capacity) return 1;

    int id = gen_id(list);
    if(id == -1) return 2;

    list->data[id] = window;
    window->win_id = id;
    window->z = list->capacity - id;
    heapify_up(list, list->size);
    list->size++;

    return 0;
}

uint8_t window_list_remove(window_list_t* list, int win_id){
    if(list->size) return 1;

    list->size--;
    if (list->size > 0)
    {
        heapify_down(list, win_id);
    }

    return 0;
}

uint8_t window_list_raise(window_list_t* list, window_t* window){
    
}