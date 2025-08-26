/// --------------------------------
/// @file physical-memory-manager.h

#ifndef PHYSICAL_MEMORY_MANAGER_H
#define PHYSICAL_MEMORY_MANAGER_H

#include <bitmap.h>

typedef void (*physical_memory_manager_failure_handle_t)(void* __this__);

typedef struct physical_memory_manager_s{
    bitmap_t bitmap;
    uint8_t is_initialized;
    uint8_t failed;
    physical_memory_manager_failure_handle_t failure;
} physical_memory_manager_t;

#endif