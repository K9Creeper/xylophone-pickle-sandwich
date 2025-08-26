/// ------======----------
/// @file paging-manager.h

#ifndef PAGING_MANAGER_H
#define PAGING_MANAGER_H

#include <kernel/data-structures/memory-management/paging-structures.h>

typedef struct paging_manager_s{
    struct heap_manager_s* heap_manager;
    struct physical_memory_manager_s* physical_memory_manager;

    page_directory_t* page_directory;

    uint8_t is_initialized;
} paging_manager_t;

#endif