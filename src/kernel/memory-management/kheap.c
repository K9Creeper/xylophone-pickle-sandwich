/// -------
/// kheap.c
/// @brief This file contains the definitions to heap init related functions.

#include "kheap.h"

#include <memory-management/heap-manager.h>
#include <memory-management/paging-manager.h>

#include <data-structures/kernel-context/kernel-context.h>

// linker.ld
extern uint32_t linkerld_endofkernel;

// kernel-main.c
extern kernel_context_t *kernel_context;

bool kheap_pre_init(void){
    uint32_t kernel_memory_end = (uint32_t)(&linkerld_endofkernel);

    heap_manager_pre_init(&kernel_context->heap_manager, kernel_memory_end);

    return kernel_context->heap_manager.status == HEAP_PRE_INITIALIZED;
}

bool kheap_init(void){
    // set as true, false, false
    heap_manager_init(&kernel_context->heap_manager, 0xC0400000, 0xC0500000, 0xCFFFFF00, true, false, false, &kernel_context->paging_manager);
    //heap_manager_init(&kernel_context->heap_manager, 0xC0400000, 0xC0500000, 0xCFFFFF00, false, false, false, &kernel_context->paging_manager);

    return kernel_context->heap_manager.status == HEAP_INITIALIZED;
}