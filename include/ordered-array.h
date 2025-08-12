/// -----------------
/// ordered-array.hpp
/// @brief This file declares the ordered_array structure and functions.

#ifndef ORDERED_ARRAY_H
#define ORDERED_ARRAY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef TYPE_T
#define TYPE_T

typedef void* type_t;

#endif

typedef bool (*ordered_array_is_less_than_handle_t)(const type_t, const type_t);

typedef struct ordered_array_s{
    type_t* array;

	uint32_t size;
    uint32_t max_size;

    ordered_array_is_less_than_handle_t is_less_than_handle;
} ordered_array_t;

extern void ordered_array_init(ordered_array_t* ordered_array, uint32_t max_size, ordered_array_is_less_than_handle_t handle);

extern void ordered_array_place(ordered_array_t* ordered_array, void *address);

extern bool ordered_array_insert(ordered_array_t* ordered_array, type_t item);

extern void ordered_array_remove(ordered_array_t* ordered_array, uint32_t index);

extern uint32_t ordered_array_size(ordered_array_t* ordered_array);

extern void ordered_array_clear(ordered_array_t* ordered_array);

// "safer" data retreival.
extern type_t ordered_array_get(ordered_array_t* ordered_array, uint32_t index);

#endif