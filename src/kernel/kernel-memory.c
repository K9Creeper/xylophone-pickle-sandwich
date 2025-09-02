/// ---------------------
/// @file kernel-memory.c

#include <stdint.h>
#include <string.h>

#include "kernel/util.h"
#include <kernel/data-structures/kernel-context.h>
#include <kernel/memory-management/heap-manager.h>

extern kernel_context_t *kernel_context;

void *kernel_malloc(uint32_t size) {
    return heap_manager_malloc(&kernel_context->heap_manager, size, 0, NULL);
}

void kernel_free(void *ptr) {
    heap_manager_free(&kernel_context->heap_manager, (uint32_t)ptr);
}

void *kernel_amalloc(uint32_t size) {
    return heap_manager_malloc(&kernel_context->heap_manager, size, 1, NULL);
}

void *kernel_calloc(uint32_t n, uint32_t size) {
    uint32_t total = n * size;
    void *ptr = kernel_amalloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void *kernel_realloc(void *ptr, uint32_t size) {
    if (!ptr) return kernel_amalloc(size);
    if (size == 0) { 
        kernel_free(ptr); 
        return NULL; 
    }

    void *new_ptr = kernel_amalloc(size);
    if (!new_ptr) return NULL;

    memcpy(new_ptr, ptr, size);
    kernel_free(ptr);
    return new_ptr;
}