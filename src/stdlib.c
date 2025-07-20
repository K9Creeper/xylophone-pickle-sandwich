#include <stdlib.h>
#include <memory.h>

#include <memory-management/heap-manager.h>

// heap-manager.c
extern heap_manager_t *current_system_heap_manager;

void* malloc(uint32_t size){
    if(!current_system_heap_manager)
        return NULL;    

    return heap_manager_malloc(current_system_heap_manager, size, false, NULL);
}

void free(void* ptr){
    if(!current_system_heap_manager)
        return;
    
    heap_manager_free(current_system_heap_manager, (uint32_t)ptr);
}

void* calloc(uint32_t n, uint32_t size) {
    uint32_t total = n * size;
    void* ptr = malloc(total);
    if (ptr)
        memset(ptr, 0, total);
    return ptr;
}

// @note: a really poor implementation (not utilizing core heap functionality / data)
void* realloc(void* ptr, uint32_t size) {
    if (!ptr)
        return malloc(size);  

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    void* new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, size);
    free(ptr);
    return new_ptr;
}