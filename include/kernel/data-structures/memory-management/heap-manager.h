/// --------------------
/// @file heap-manager.h

#ifndef HEAP_MANAGER_H
#define HEAP_MANAGER_H

#include <ordered-array.h>

typedef enum {
    HEAP_UNINITIALIZED,
    HEAP_BOOTSTRAPED,
    HEAP_INITIALIZED
} heap_status_enum_t;

typedef uint8_t heap_status_t; // Used for storage

typedef struct heap_manager_s
{
    struct paging_manager_s *paging_manager;

    uint32_t placement_address;

    heap_status_t status;

    uint8_t is_supervisor;
    uint8_t is_readonly;

    uint8_t use_identity_mapping;

    uint32_t start_address;
    uint32_t end_address;
    uint32_t max_end_address;
    
    ordered_array_t heap_array;
} heap_manager_t;

#endif