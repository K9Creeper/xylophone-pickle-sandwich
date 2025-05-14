/// -----------
/// bitmap.hpp
/// @brief This file declares the template based Bitmap class.

#pragma once

#include <stdint.h>

template <typename type_t>
class Bitmap
{
protected:
    type_t *array;

    uint32_t count;
    uint32_t memory_size;
    
    uint32_t sizeof_type_t;
    uint32_t byte_per_entry;
    
public:

    void Create();

    void Place(void *addr, uint32_t size = 0);

    void Set_Count(uint32_t count);
    void Set_Memory_Size(uint32_t size);

    type_t& operator[](uint32_t i);

    type_t& Get(uint32_t i);

    void Set(uint32_t i, type_t val);

    uint32_t Get_Count()const;
    uint32_t Get_Memory_Size()const;

    uint32_t Get_Type_Size() const;
    uint32_t Get_Bytes_Per_Entry() const;

    uint32_t Index_From_Bit(uint32_t i)const;
    uint32_t Offset_From_Bit(uint32_t i)const;

    virtual void Clear();
};