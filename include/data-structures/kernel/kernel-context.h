/// ----------------
/// kernel-context.h
/// @brief This file defines the structure of the kernel context.

#ifndef KERNEL_CONTEXT_H
#define KERNEL_CONTEXT_H

#include <stdint.h>

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

#include <disk-manager/disk-manager.h>
#include <filesystem/filesystem.h>

typedef struct kernel_context_video_state_s{
    bool is_text_mode;
    
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint32_t bpp;

    uint32_t lfb;
}kernel_context_video_state_t;

typedef struct kernel_context_s{
    paging_manager_t paging_manager;
    heap_manager_t heap_manager;

    uint32_t physical_memory_size;

    disk_manager_t disk_manager;
    filesystem_t filesystem;
    
    kernel_context_video_state_t video_state;
}kernel_context_t;

#endif