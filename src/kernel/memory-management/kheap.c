/// -------
/// kheap.c
/// @brief This file contains the definitions to heap init related functions.

#include "kheap.h"

#include <memory-management/heap-manager.h>
#include <memory-management/paging-manager.h>

// linker.ld
extern uint32_t linkerld_endofkernel;

// heap-manager.c
extern heap_manager_t* current_system_heap_manager;

// paging.c
extern paging_manager_t kernel_paging_manager;

heap_manager_t kernel_heap_manager;

bool kheap_pre_init(void){
    uint32_t kernel_memory_end = (uint32_t)(&linkerld_endofkernel);

    current_system_heap_manager = &kernel_heap_manager;

    heap_manager_pre_init(&kernel_heap_manager, kernel_memory_end);

    return kernel_heap_manager.status == HEAP_PRE_INITIALIZED;
}

bool kheap_init(void){
    // set as true, false, false
    heap_manager_init(&kernel_heap_manager, 0xC0400000, 0xC0500000, 0xCFFFFF00, true, false, false, &kernel_paging_manager);
    //heap_manager_init(&kernel_heap_manager, 0xC0400000, 0xC0500000, 0xCFFFFF00, false, false, false, &kernel_paging_manager);

    return kernel_heap_manager.status == HEAP_INITIALIZED;
}