/// -------------------
/// heap-structures.hpp
/// This file contains the definition of heap-related data structures.
#pragma once

#include <stdint.h>

struct Header
{
    uint32_t magic;

    bool is_hole;
    uint32_t size;
};

struct Footer
{
    uint32_t magic;
    Header* header;
};

typedef void* Heap_Entry;

#include "../../helpers/ordered-array.hpp"

class Heap_Array : Ordered_Array<Heap_Entry>
{
    public:
        bool Insert(Heap_Entry entry);
};