/// ----------------
/// paging-manager.h
/// @brief This file contains the definition of the paging_manager structure.

#ifndef PAGING_MANAGER_H
#define PAGING_MANAGER_H

#include <stdbool.h>
#include <data-structures/memory-management/paging-structures.h>

#define PAGE_SIZE 0x1000

typedef struct paging_manager_s{
    struct heap_manager_s* heap_manager;

    page_directory_t* page_directory;

    bool is_initialized;
    bool is_enabled;

} paging_manager_t;

extern void paging_manager_init(paging_manager_t* paging_manager, page_directory_t* page_directory, struct heap_manager_s* heap_manager, bool should_clear);

extern bool paging_manager_swap_system_page_directory(uint32_t cr3, bool is_physical);

extern bool paging_manager_set_as_system_paging(paging_manager_t* paging_manager, bool is_physical);

extern void paging_manager_swap_page_directory(paging_manager_t* paging_manager, page_directory_t* page_directory);

extern uint32_t paging_manager_get_physical_address(paging_manager_t* paging_manager, uint32_t virtual_address);

extern void paging_manager_enable(paging_manager_t* paging_manager);

extern void paging_manager_identity_allocate_single(paging_manager_t* paging_manager, uint32_t address, bool is_kernel, bool is_writeable);
extern void paging_manager_identity_allocate_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable);

extern void paging_manager_allocate_single(paging_manager_t* paging_manager, uint32_t address, bool is_kernel, bool is_writeable);
extern void paging_manager_allocate_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable);

extern void paging_manager_free_single(paging_manager_t* paging_manager, uint32_t address, bool should_free);
extern void paging_manager_free_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, bool should_free);

// NOTE: special function, used in specfic cases only
extern void paging_manager_allocate_to_physical(paging_manager_t* paging_manager, uint32_t virtual_address, uint32_t physical_address, bool is_kernel, bool is_writeable);

#endif