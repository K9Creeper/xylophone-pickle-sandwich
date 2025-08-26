/// -----------------------
/// @file ordered-array.hpp

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <stddef.h>
#include <stdint.h>

#ifndef TYPE_T
#define TYPE_T

typedef void* type_t;

#endif

typedef uint8_t (*ordered_array_is_less_than_handle_t)(const type_t, const type_t);

typedef struct ordered_array_s{
    type_t* array;

	uint32_t size;
    uint32_t max_size;

    ordered_array_is_less_than_handle_t is_less_than_handle;
} ordered_array_t;

extern void ordered_array_init(ordered_array_t* ordered_array, uint32_t max_size, ordered_array_is_less_than_handle_t handle);

extern void ordered_array_place(ordered_array_t* ordered_array, void *address);

extern uint8_t ordered_array_insert(ordered_array_t* ordered_array, type_t item);

extern void ordered_array_remove(ordered_array_t* ordered_array, uint32_t index);

static inline uint32_t ordered_array_size(ordered_array_t* oa) {
    return oa ? oa->size : (uint32_t)-1;
}

static inline type_t ordered_array_get(ordered_array_t* oa, uint32_t index) {
    return oa->array[index];
}

#endif