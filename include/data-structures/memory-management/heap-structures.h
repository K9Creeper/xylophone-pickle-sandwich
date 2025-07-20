/// -----------------
/// heap-structures.h
/// @brief This file contains the definition of heap-related data structures.

#ifndef HEAP_STRUCTURES_H
#define HEAP_STRUCTURES_H

#include <stdint.h>

#define HEAP_MAGIC 0xDEADBEEF

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000

typedef struct heap_header_s{
    uint32_t magic;
    bool is_hole;
    uint32_t size;
} heap_header_t;

typedef struct heap_footer_s
{
    uint32_t magic;
    heap_header_t* header;
} heap_footer_t;

#endif