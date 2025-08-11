/// -----------
/// mini-heap.h
/// @brief This file defines the structure and declares the functions that control a mini-heap.

#ifndef MINI_HEAP_H
#define MINI_HEAP_H

#include <stdint.h>
#include <stdbool.h>

#ifndef TYPE_T
#define TYPE_T

typedef void* type_t;

#endif

typedef bool (*mini_heap_is_less_than_handle_t)(const type_t, const type_t);

typedef struct mini_heap_s {
    type_t* array;
    uint32_t size;
    uint32_t capacity;
    mini_heap_is_less_than_handle_t cmp;
} mini_heap_t;

extern void mini_heap_init(mini_heap_t* heap, type_t* address, uint32_t capacity, mini_heap_is_less_than_handle_t cmp);
extern void mini_heap_insert(mini_heap_t* heap, type_t item);
extern type_t mini_heap_pop(mini_heap_t* heap);
extern type_t mini_heap_peek(mini_heap_t* heap);

#endif