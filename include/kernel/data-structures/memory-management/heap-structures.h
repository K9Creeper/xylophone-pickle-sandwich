/// -----------------------
/// @file heap-structures.h

#ifndef HEAP_STRUCTURES_H
#define HEAP_STRUCTURES_H

#include <stdint.h>

#define HEAP_MAGIC 0xDEADBEEF

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000

typedef struct heap_header_s
{
    uint32_t magic;
    uint32_t size;              
    uint8_t is_hole;            
    uint8_t padding[3];
} heap_header_t;

typedef struct heap_footer_s
{
    uint32_t magic;
    heap_header_t *header;
} heap_footer_t;

#endif