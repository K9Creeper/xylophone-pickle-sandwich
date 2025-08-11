/// -----------
/// mini-heap.c
/// @brief This file defines the functions of the mini-heap.

#include <mini-heap.h>

#include <stddef.h>

void mini_heap_init(mini_heap_t* heap, type_t* address, uint32_t capacity, mini_heap_is_less_than_handle_t cmp){
    heap->array = address;
    heap->size = 0;
    heap->capacity = capacity;
    heap->cmp = cmp;
}

void mini_heap_insert(mini_heap_t* heap, type_t item){
    if (heap->size == heap->capacity) return; // or resize
    heap->array[heap->size] = item;
    uint32_t i = heap->size++;
    while (i > 0) {
        uint32_t parent = (i - 1) / 2;
        if (!heap->cmp(heap->array[i], heap->array[parent])) break;
        type_t tmp = heap->array[i];
        heap->array[i] = heap->array[parent];
        heap->array[parent] = tmp;
        i = parent;
    }
}

type_t mini_heap_pop(mini_heap_t* heap){
    if (heap->size == 0) return NULL;
    type_t result = heap->array[0];
    heap->array[0] = heap->array[--heap->size];
    uint32_t i = 0;
    while (true) {
        uint32_t left = 2*i + 1;
        uint32_t right = 2*i + 2;
        uint32_t smallest = i;

        if (left < heap->size && heap->cmp(heap->array[left], heap->array[smallest])) smallest = left;
        if (right < heap->size && heap->cmp(heap->array[right], heap->array[smallest])) smallest = right;

        if (smallest == i) break;

        type_t tmp = heap->array[i];
        heap->array[i] = heap->array[smallest];
        heap->array[smallest] = tmp;

        i = smallest;
    }
    return result;
}

type_t mini_heap_peek(mini_heap_t* heap){
    return heap->size > 0 ? heap->array[0] : NULL;
}