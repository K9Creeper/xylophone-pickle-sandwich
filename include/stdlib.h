#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

void* malloc(uint32_t size);
void  free(void* ptr);
void* amalloc(uint32_t size);
void* calloc(uint32_t num, uint32_t size);
void* realloc(void* ptr, uint32_t new_size);

#endif

// @note: this is a custom stdlib, built for after the memory management setup in kernel-main.c