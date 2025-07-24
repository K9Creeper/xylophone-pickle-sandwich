/// --------
/// paging.c
/// @brief This file contains the definitions of kernel paging initialization functions.

#include "paging.h"

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

#include <stdio.h>

// heap-manager.c
extern heap_manager_t *current_system_heap_manager;

// paging-manager.c
extern paging_manager_t* current_system_paging_manager;

// kheap.c
extern heap_manager_t kernel_heap_manager;

paging_manager_t kernel_paging_manager;

bool paging_init(void)
{
    page_directory_t* pd = heap_manager_malloc(&kernel_heap_manager, sizeof(page_directory_t), true, NULL);
    paging_manager_init(
        &kernel_paging_manager,
        pd,
        &kernel_heap_manager,
        true
    );

    current_system_heap_manager = kernel_paging_manager.heap_manager;
    current_system_paging_manager = &kernel_paging_manager;

    // true, true for the allocs

    paging_manager_allocate_range(&kernel_paging_manager, 0xC0000000, 0xC0000000 + 0x400000, true, true);
    //paging_manager_allocate_range(&kernel_paging_manager, 0xC0000000, 0xC0000000 + 0x400000, false, true);

    paging_manager_set_as_system_paging(&kernel_paging_manager, false);
    paging_manager_enable(&kernel_paging_manager);

    paging_manager_identity_allocate_range(&kernel_paging_manager, 0x0, 0x100000, true, true);
    //paging_manager_identity_allocate_range(&kernel_paging_manager, 0x0, 0x100000, false, true);

    return kernel_paging_manager.is_initialized && kernel_paging_manager.is_enabled;
}