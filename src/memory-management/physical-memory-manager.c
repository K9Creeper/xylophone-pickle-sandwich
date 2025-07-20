/// -------------------------
/// physical-memory-manager.c
/// @brief This file contains the definition of the physical_memeory_manager structure methods.

#include <memory-management/physical-memory-manager.h>

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

extern heap_manager_t *current_system_heap_manager;

void physical_memory_manager_init(physical_memory_manager_t* physical_memory_manager, uint32_t memory_size){
    if(physical_memory_manager->is_initialized)
        return; 

    bitmap_init(&physical_memory_manager->bitmap, sizeof(uint32_t));

    const int count = memory_size / PAGE_SIZE;
    const int size = (count / physical_memory_manager->bitmap.bits_per_entry);

    bitmap_place(&physical_memory_manager->bitmap, heap_manager_malloc(current_system_heap_manager, size, false, NULL), size);
    bitmap_clear(&physical_memory_manager->bitmap);

    physical_memory_manager->is_initialized = true;
}

uint32_t physical_memory_manager_allocate_block(physical_memory_manager_t* physical_memory_manager){
    uint32_t free_block;
    bool found = physical_memory_manager_get_first_free_block(physical_memory_manager, &free_block);

    uint32_t index = bitmap_index_from_bit(&physical_memory_manager->bitmap, free_block);
    uint32_t offset = bitmap_offset_from_bit(&physical_memory_manager->bitmap, free_block);

    physical_memory_manager->bitmap.array[index] |= (0x1 << offset);

    return free_block;
}

void physical_memory_manager_free_block(physical_memory_manager_t* physical_memory_manager, uint32_t block_number){
    uint32_t index = bitmap_index_from_bit(&physical_memory_manager->bitmap, block_number);
    uint32_t offset = bitmap_offset_from_bit(&physical_memory_manager->bitmap, block_number);

    physical_memory_manager->bitmap.array[index] &= ~(0x1 << offset);
}

bool physical_memory_manager_get_first_free_block(physical_memory_manager_t* physical_memory_manager, uint32_t* out){
    for(uint32_t i = 0; i < physical_memory_manager->bitmap.bit_count; i++) {
        if(!((physical_memory_manager->bitmap.array[bitmap_index_from_bit(&physical_memory_manager->bitmap, i)] >> (bitmap_offset_from_bit(&physical_memory_manager->bitmap, i))) & 0x1))
        {    
            *out = i;
            return true;
        }
    }
    return false;
}