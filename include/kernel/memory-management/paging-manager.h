/// ----------------------
/// @file paging-manager.h

#ifndef _PAGING_MANAGER_H
#define _PAGING_MANAGER_H

#include <kernel/data-structures/memory-management/paging-manager.h>

extern void paging_manager_init(paging_manager_t* paging_manager, page_directory_t* page_directory, struct heap_manager_s* heap_manager, struct physical_memory_manager_s* physical_memory_manager, uint8_t should_clear);

extern uint8_t paging_manager_set_system_paging(paging_manager_t *paging_manager, uint8_t is_physical);
extern uint8_t paging_manager_set_cr3(paging_manager_t *paging_manager, uint32_t cr3, uint8_t is_physical);

extern void paging_manager_enable(paging_manager_t *paging_manager);

extern uint32_t paging_manager_get_physical_address(paging_manager_t *paging_manager, uint32_t virtual_address);

extern void paging_manager_identity_allocate_single(paging_manager_t* paging_manager, uint32_t address, uint8_t is_kernel, uint8_t is_writeable);
extern void paging_manager_identity_allocate_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, uint8_t is_kernel, uint8_t is_writeable);

extern void paging_manager_allocate_single(paging_manager_t* paging_manager, uint32_t address, uint8_t is_kernel, uint8_t is_writeable);
extern void paging_manager_allocate_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, uint8_t is_kernel, uint8_t is_writeable);

extern void paging_manager_free_single(paging_manager_t* paging_manager, uint32_t address, uint8_t should_free);
extern void paging_manager_free_range(paging_manager_t* paging_manager, uint32_t start_address, uint32_t end_address, uint8_t should_free);

// NOTE: special function, used in specfic cases only
extern void paging_manager_allocate_to_physical(paging_manager_t* paging_manager, uint32_t virtual_address, uint32_t physical_address, uint8_t is_kernel, uint8_t is_writeable);

#endif