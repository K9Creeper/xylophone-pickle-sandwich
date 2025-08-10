/// -----
/// pmm.c
/// @brief This file contains the definitions to phyical memory management init related functuins.

#include "pmm.h"

#include <memory-management/physical-memory-manager.h>
#include <data-structures/kernel/kernel-context.h>

// kernel-main.c
extern kernel_context_t *kernel_context;

// paging-manager.c
extern physical_memory_manager_t system_physical_memory_manager;

bool pmm_init(uint32_t size){
    physical_memory_manager_init(&system_physical_memory_manager, size, &kernel_context->heap_manager);

    return system_physical_memory_manager.is_initialized;
}