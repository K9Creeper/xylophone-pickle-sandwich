#include <kernel/stdlib.h>
#include <memory.h>

#include <memory-management/heap-manager.h>
#include <data-structures/kernel-context/kernel-context.h>

// kernel-main.c
extern kernel_context_t *kernel_context;

void* kernel_malloc(uint32_t size){
    return heap_manager_malloc(&kernel_context->heap_manager, size, false, NULL);
}

void kernel_free(void* ptr){
    heap_manager_free(&kernel_context->heap_manager, (uint32_t)ptr);
}

void* kernel_amalloc(uint32_t size){
    return heap_manager_malloc(&kernel_context->heap_manager, size, true, NULL);
}

void* kernel_calloc(uint32_t n, uint32_t size) {
    uint32_t total = n * size;
    void* ptr = kernel_amalloc(total);
    if (ptr)
        memset(ptr, 0, total);
    return ptr;
}

// NOTE: a really poor implementation (not utilizing core heap functionality / data)
void* kernel_realloc(void* ptr, uint32_t size) {
    if (!ptr)
        return kernel_amalloc(size);  

    if (size == 0) {
        kernel_free(ptr);
        return NULL;
    }

    void* new_ptr = kernel_amalloc(size);
    if (!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, size);
    kernel_free(ptr);
    return new_ptr;
}