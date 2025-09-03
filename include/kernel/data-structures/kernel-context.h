/// ----------------------
/// @file kernel-context.h

#ifndef KERNEL_CONTEXT_H
#define KERNEL_CONTEXT_H

#include <stdint.h>

#include <kernel/data-structures/memory-management/heap-manager.h>
#include <kernel/data-structures/memory-management/physical-memory-manager.h>
#include <kernel/data-structures/memory-management/paging-manager.h>

typedef struct kernel_context_video_state_s
{
    uint8_t is_text_mode;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;

    uint32_t lfb;
} kernel_context_video_state_t;

typedef struct kernel_context_memory_info_s
{
    uint32_t reserved_memory;
    uint32_t useable_memory;
} kernel_context_memory_info_t;

typedef struct kernel_context_s
{
    heap_manager_t heap_manager;
    paging_manager_t paging_manager;
    physical_memory_manager_t system_physical_memory_manager;
    
    kernel_context_memory_info_t memory_info;
    kernel_context_video_state_t video_state;
} kernel_context_t;

#endif