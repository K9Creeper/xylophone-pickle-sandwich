/// -------------------
/// heap-structures.hpp
/// This file contains the definition of heap-related data structures.
#pragma once

#include <stdint.h>

#define HEAP_MAGIC 0xDEADBEEF
#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MIN_SIZE 0x70000

struct Heap_Header
{
    uint32_t magic;

    bool is_hole;
    uint32_t size;
};

struct Heap_Footer
{
    uint32_t magic;
    Heap_Header* header;
};

typedef void* Heap_Entry;

#include "../../helpers/ordered-array.hpp"

class Heap_Array : public Ordered_Array<Heap_Entry>
{
    public:
        bool Insert(Heap_Entry entry);
};