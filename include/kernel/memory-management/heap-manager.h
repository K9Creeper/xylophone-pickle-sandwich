/// --------------------
/// @file heap-manager.h

#ifndef _HEAP_MANAGER_H
#define _HEAP_MANAGER_H

#include <kernel/data-structures/memory-management/heap-manager.h>

extern void heap_manager_bootstrap(heap_manager_t* heap, uint32_t end);
extern void heap_manager_init(heap_manager_t* heap, uint32_t start_address, uint32_t end_address, uint32_t max_end_address, uint8_t is_supervisor, uint8_t is_readonly, uint8_t use_identity_mapping, struct paging_manager_s* paging_manager);

extern void heap_manager_free_all(heap_manager_t *heap);

extern void* heap_manager_malloc(heap_manager_t* heap, uint32_t size, uint8_t should_align, uint32_t *physical_address);
extern void heap_manager_free(heap_manager_t* heap, uint32_t address);

#endif