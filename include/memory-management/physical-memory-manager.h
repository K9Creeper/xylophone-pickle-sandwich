/// -------------------------
/// physical-memory-manager.h
/// @brief This file contains the declarations of the physical_memeory_manager structure.

#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H

#include <stdbool.h>
#include <bitmap.h>

#include <memory-management/heap-manager.h>

typedef struct physical_memory_manager_s{
    bitmap_t bitmap;
    bool is_initialized;

    bool failed;
} physical_memory_manager_t;

extern void physical_memory_manager_init(physical_memory_manager_t* physical_memory_manager, uint32_t memory_size, heap_manager_t* heap_manager);

extern uint32_t physical_memory_manager_allocate_block(physical_memory_manager_t* physical_memory_manager);
extern void physical_memory_manager_free_block(physical_memory_manager_t* physical_memory_manager, uint32_t block_number);

extern bool physical_memory_manager_get_first_free_block(physical_memory_manager_t* physical_memory_manager, uint32_t* out); 

extern bool physical_memory_manager_is_valid_block(physical_memory_manager_t* physical_memory_manager, uint32_t block);

#endif