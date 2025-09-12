/// -------------------
/// @file window-list.c

#include <kernel/window-server/window-list.h>

#include <stddef.h>

static inline int gen_id(window_list_t *list)
{
    if (list->free_count == 0)
        return -1;
    return list->free_ids[--list->free_count]; // pop from free list
}

static inline void free_id(window_list_t *list, int id)
{
    list->free_ids[list->free_count++] = id; // push back into free list
}

static inline void swap(window_list_t **a, window_list_t **b)
{
    window_list_t *tmp = *a;
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
        idx = smallest;
    }
}

void window_list_init(window_list_t *list, window_t **window_data, int *free_ids, int capacity)
{
    list->data = window_data;
    list->free_ids = free_ids;
    for (int i = 0; i < capacity; i++)
    {
        list->free_ids[i] = i;
    }
    list->free_count = capacity;

    list->size = 0;
    list->capacity = capacity;
}

uint8_t window_list_add(window_list_t* list, window_t* window) {
    if (list->size >= list->capacity) return 1;

    int id = gen_id(list);
    if (id == -1) return 2;

    window->win_id = id;
    window->z = list->capacity - list->size;

    list->data[list->size] = window;

    heapify_up(list, list->size);

    list->size++;
    return 0;
}

uint8_t window_list_remove(window_list_t* list, int win_id) {
    if (list->size == 0) return 1;

    int idx = -1;
    for (int i = 0; i < list->size; i++) {
        if (list->data[i]->win_id == win_id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return 2;

    list->size--;
    if (idx != list->size) {
        list->data[idx] = list->data[list->size];
        heapify_down(list, idx);
        heapify_up(list, idx);
    }

    free_id(list, win_id);

    return 0;
}

uint8_t window_list_raise(window_list_t* list, int win_id) {
    int idx = -1;
    for (int i = 0; i < list->size; i++) {
        if (list->data[i]->win_id == win_id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return 1;

    list->data[idx]->z = list->capacity;

    heapify_up(list, idx);
    return 0;
}