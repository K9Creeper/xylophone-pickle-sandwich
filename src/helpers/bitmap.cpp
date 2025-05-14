/// -----------
/// bitmap.cpp
/// @brief This file defines the core functions of the template based Bitmap class.

#include "bitmap.hpp"

#include "memory.h"

template <typename type_t>
void Bitmap<type_t>::Create()
{
    array = nullptr;
    count = 0;
    memory_size = 0;

    sizeof_type_t = sizeof(type_t);
    byte_per_entry = sizeof_type_t * 8;
}

template <typename type_t>
void Bitmap<type_t>::Place(void *address, uint32_t size)
{
    this->array = reinterpret_cast<type_t *>(address);
    Set_Memory_Size(size);
}

template <typename type_t>
void Bitmap<type_t>::Set_Count(uint32_t count)
{
    this->count = count;
    this->memory_size = count / byte_per_entry;
}

template <typename type_t>
void Bitmap<type_t>::Set_Memory_Size(uint32_t memory_size)
{
    this->memory_size = memory_size;
    this->count = memory_size * byte_per_entry;
}

template <typename type_t>
type_t &Bitmap<type_t>::operator[](uint32_t i)
{
    return this->array[i];
}

template <typename type_t>
type_t& Bitmap<type_t>::Get(uint32_t i)
{
    return this->array[i];
}

template <typename type_t>
void Bitmap<type_t>::Set(uint32_t i, type_t val)
{
    this->array[i] = val;
}

template <typename type_t>
uint32_t Bitmap<type_t>::Get_Count() const
{
    return this->count;
}

template <typename type_t>
uint32_t Bitmap<type_t>::Get_Memory_Size() const
{
    return this->memory_size;
}

template <typename type_t>
uint32_t Bitmap<type_t>::Get_Type_Size() const
{
    return sizeof_type_t;
}

template <typename type_t>
uint32_t Bitmap<type_t>::Get_Bytes_Per_Entry() const
{
    return byte_per_entry;
}
template <typename type_t>
uint32_t Bitmap<type_t>::Index_From_Bit(uint32_t i) const{
    return i / byte_per_entry;
}

template <typename type_t>

uint32_t Bitmap<type_t>::Offset_From_Bit(uint32_t i) const{
    return i % byte_per_entry;
}

template <typename type_t>
void Bitmap<type_t>::Clear()
{
    memset(reinterpret_cast<unsigned char *>(array), 0, count * sizeof(type_t));
}

template class Bitmap<uint32_t>;