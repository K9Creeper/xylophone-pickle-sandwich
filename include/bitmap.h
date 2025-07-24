/// --------
/// bitmap.h
/// @brief This file declares the template based bitmap structure.

#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>
#include <stddef.h>

typedef struct bitmap_s {
    uint32_t* array;

    uint32_t bit_count;     // number of bits the bitmap tracks
    uint32_t memory_size;   // number of bytes allocated to `array`

    uint32_t sizeof_type_t; // size in bytes of each array entry
    uint32_t bits_per_entry; // = sizeof_type_t * 8; bits per entry
} bitmap_t;

extern void bitmap_init(bitmap_t* bitmap, uint32_t sizeof_type_t);

extern void bitmap_place(bitmap_t* bitmap, void* placement, uint32_t memory_size);

extern void bitmap_set_bit_count(bitmap_t* bitmap, uint32_t bit_count);
extern void bitmap_set_memory_size(bitmap_t* bitmap, uint32_t memory_size);

extern uint32_t bitmap_index_from_bit(bitmap_t* bitmap, uint32_t idx);
extern uint32_t bitmap_offset_from_bit(bitmap_t* bitmap, uint32_t idx);

extern void bitmap_clear(bitmap_t* bitmap);

#endif