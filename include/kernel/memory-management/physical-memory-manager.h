/// -------------------------------
/// @file physical-memory-manager.h

#ifndef _PHYSICAL_MEMORY_MANAGER_H
#define _PHYSICAL_MEMORY_MANAGER_H
#include <stdint.h>

#include <kernel/data-structures/memory-management/physical-memory-manager.h>

#include <kernel/data-structures/memory-management/heap-manager.h>

extern void physical_memory_manager_init(physical_memory_manager_t *physical_memory_manager, uint32_t memory_size, physical_memory_manager_failure_handle_t failure_handle, heap_manager_t* heap_manager);

extern uint32_t physical_memory_manager_allocate_block(physical_memory_manager_t *physical_memory_manager);
extern void physical_memory_manager_free_block(physical_memory_manager_t *physical_memory_manager, uint32_t block_number);

extern uint8_t physical_memory_manager_get_first_free_block(physical_memory_manager_t *physical_memory_manager, uint32_t *out);

static inline uint8_t physical_memory_manager_is_valid_block(physical_memory_manager_t *physical_memory_manager, uint32_t block)
{
    uint32_t index = bitmap_index_from_bit(&physical_memory_manager->bitmap, block);
    uint32_t max_entry_count = physical_memory_manager->bitmap.memory_size / physical_memory_manager->bitmap.sizeof_type_t;
    return index < max_entry_count;
}

static inline double physical_memory_manager_usage(physical_memory_manager_t *physical_memory_manager){
    return (double)((double)physical_memory_manager->block_used / physical_memory_manager->bitmap.bit_count);
}

#endif