/// -------------------
/// heap-structures.cpp
/// This file contains the definitions of heap-related data structures' functions.

#include "heap-structures.hpp"

static bool Is_Less_Than(Entry a, Entry b)
{
    return ((reinterpret_cast<Header *>(a))->size < (reinterpret_cast<Header *>(b))->size);
}

bool HeapOrderedArray::Insert(Entry item)
{
    uint32_t i;

    static Entry tmp;
    static Entry tmp2;

    i = 0;

    while (i < this->size && Is_Less_Than(this->array[i], item))
    {
        i++;
    }

    if (i + 1 > this->max_size)
        return false;

    if (i == this->size)
    {
        this->array[this->size] = item;

        this->size++;
    }
    else
    {
        tmp = this->array[i];

        this->array[i] = item;

        while (i < this->size)
        {
            i++;

            tmp2 = this->array[i];

            this->array[i] = tmp;

            tmp = tmp2;
        }

        this->size++;
    }

    return true;
}