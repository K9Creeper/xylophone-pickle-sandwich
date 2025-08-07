#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void* kernel_malloc(uint32_t size);
void  kernel_free(void* ptr);
void* kernel_amalloc(uint32_t size);
void* kernel_calloc(uint32_t num, uint32_t size);
void* kernel_realloc(void* ptr, uint32_t new_size);

#endif

// @note: this is a custom stdlib, built for after the memory management setup in kernel-main.c