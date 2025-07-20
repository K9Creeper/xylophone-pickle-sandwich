/// --------
/// bitmap.c
/// @brief This file defines the core functions of the bitmap structure.

#include <bitmap.h>
#include <memory.h> 

#define BITS_PER_BYTE 8

void bitmap_init(bitmap_t* bitmap, uint32_t sizeof_type_t) {
    if (!bitmap)
        return;

    bitmap->array = NULL;
    bitmap->bit_count = 0;
    bitmap->memory_size = 0;

    bitmap->sizeof_type_t = sizeof_type_t;
    bitmap->bits_per_entry = sizeof_type_t * BITS_PER_BYTE;
}

void bitmap_place(bitmap_t* bitmap, void* placement, uint32_t memory_size) {
    if (!bitmap)
        return;

    bitmap->array = placement;
    bitmap_set_memory_size(bitmap, memory_size);
}

void bitmap_set_bit_count(bitmap_t* bitmap, uint32_t bit_count) {
    if (!bitmap)
        return;

    bitmap->bit_count = bit_count;

    // Expected to always be divisible...
    uint32_t entry_count = bit_count / bitmap->bits_per_entry;

    bitmap->memory_size = entry_count * bitmap->sizeof_type_t;
}

void bitmap_set_memory_size(bitmap_t* bitmap, uint32_t memory_size) {
    if (!bitmap)
        return;

    bitmap->memory_size = memory_size;

    // Expected to always be divisible...
    uint32_t entry_count = memory_size / bitmap->sizeof_type_t;

    bitmap->bit_count = entry_count * bitmap->bits_per_entry;
}

uint32_t bitmap_index_from_bit(bitmap_t* bitmap, uint32_t idx) {
    if (!bitmap || bitmap->bits_per_entry == 0)
        return (uint32_t)-1;

    return idx / bitmap->bits_per_entry;
}

uint32_t bitmap_offset_from_bit(bitmap_t* bitmap, uint32_t idx) {
    if (!bitmap || bitmap->bits_per_entry == 0)
        return (uint32_t)-1;

    return idx % bitmap->bits_per_entry;
}

void bitmap_clear(bitmap_t* bitmap) {
    if (bitmap && bitmap->array)
        memset((uint8_t*)bitmap->array, 0, bitmap->memory_size);
}
