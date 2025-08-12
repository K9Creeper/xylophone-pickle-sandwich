/// ---------------
/// ordered-array.c
/// @brief This file defines core functions of the ordered_array structure.

#include <ordered-array.h>

#include <memory.h>

void ordered_array_init(ordered_array_t* oa, uint32_t max_size, ordered_array_is_less_than_handle_t cmp) {
    if (!oa) return;
    oa->array = NULL;
    oa->size = 0;
    oa->max_size = max_size;
    oa->is_less_than_handle = cmp;
}

void ordered_array_place(ordered_array_t* oa, void* address) {
    if (!oa) return;
    oa->array = (type_t*) address;

    uint8_t* ptr = (uint8_t*) address;
    for (uint32_t i = 0; i < oa->max_size * sizeof(type_t); i++) {
        ptr[i] = 0;
    }

    oa->size = 0;
}

bool ordered_array_insert(ordered_array_t* oa, type_t item) {
    if (!oa || !oa->is_less_than_handle || oa->size >= oa->max_size)
        return false;

    uint32_t left = 0, right = oa->size;
    while (left < right) {
        uint32_t mid = left + (right - left) / 2;
        if (oa->is_less_than_handle(oa->array[mid], item))
            left = mid + 1;
        else
            right = mid;
    }

    for (uint32_t i = oa->size; i > left; i--) {
        oa->array[i] = oa->array[i - 1];
    }

    oa->array[left] = item;
    oa->size++;
    return true;
}

void ordered_array_remove(ordered_array_t* oa, uint32_t index) {
    if (!oa || index >= oa->size) return;

    for (uint32_t i = index; i < oa->size - 1; i++) {
        oa->array[i] = oa->array[i + 1];
    }
    oa->size--;
}

uint32_t ordered_array_size(ordered_array_t* oa) {
    return oa ? oa->size : (uint32_t)-1;
}

void ordered_array_clear(ordered_array_t* oa){
    if (!oa || !oa->array) return;
    memset((uint8_t*)oa->array, 0, oa->max_size * sizeof(type_t));
    oa->size = 0;
}

type_t ordered_array_get(ordered_array_t* oa, uint32_t index) {
    if (!oa || index >= oa->size) return NULL;
    return oa->array[index];
}