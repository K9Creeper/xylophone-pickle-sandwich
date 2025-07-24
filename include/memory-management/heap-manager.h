/// --------------
/// heap-manager.h
/// @brief This file contains the definition of the heap_manager structure.

#ifndef HEAP_MANAGER_H
#define HEAP_MANAGER_H

#include <stdbool.h>
#include <data-structures/memory-management/heap-structures.h>

#include <ordered-array.h>

typedef enum {
    HEAP_UNINITIALIZED,
    HEAP_PRE_INITIALIZED,
    HEAP_INITIALIZED
} heap_status_enum_t;

typedef uint8_t heap_status_t; // Used for storage

typedef struct heap_manager_s{
    struct paging_manager_s* paging_manager;

    uint32_t placement_address;

    heap_status_t status;

    bool is_supervisor;
    bool is_readonly;

    bool use_identity_mapping;

    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_end_address;

    ordered_array_t heap_array;
} heap_manager_t;

extern void heap_manager_pre_init(heap_manager_t* heap, uint32_t end);
extern void heap_manager_init(heap_manager_t* heap, uint32_t start_address, uint32_t end_address, uint32_t max_end_address, bool is_supervisor, bool is_readonly, bool use_identity_mapping, struct paging_manager_s* paging_manager);

extern void* heap_manager_malloc(heap_manager_t* heap, uint32_t size, bool should_align, uint32_t *physical_address);
extern void heap_manager_free(heap_manager_t* heap, uint32_t address);

#endif