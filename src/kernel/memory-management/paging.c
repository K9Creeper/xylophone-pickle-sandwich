/// --------
/// paging.c
/// @brief This file contains the definitions of kernel paging initialization functions.

#include "paging.h"

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

#include <data-structures/kernel-context/kernel-context.h>

// kernel-main.c
extern kernel_context_t *kernel_context;

bool paging_init(void)
{
    page_directory_t* pd = heap_manager_malloc(&kernel_context->heap_manager, sizeof(page_directory_t), true, NULL);
    paging_manager_init(
        &kernel_context->paging_manager,
        pd,
        &kernel_context->heap_manager,
        true
    );

    // true, true for the allocs

    paging_manager_allocate_range(&kernel_context->paging_manager, 0xC0000000, 0xC0000000 + 0x400000, true, true);
    //paging_manager_allocate_range(&kernel_context->paging_manager, 0xC0000000, 0xC0000000 + 0x400000, false, true);

    paging_manager_set_as_system_paging(&kernel_context->paging_manager, false);
    paging_manager_enable(&kernel_context->paging_manager);

    paging_manager_identity_allocate_range(&kernel_context->paging_manager, 0x0, 0x100000, true, true);
    //paging_manager_identity_allocate_range(&kernel_context->paging_manager, 0x0, 0x100000, false, true);

    return kernel_context->paging_manager.is_initialized && kernel_context->paging_manager.is_enabled;
}