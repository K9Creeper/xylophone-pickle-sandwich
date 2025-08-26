/// --------------
/// @file bitmap.c

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