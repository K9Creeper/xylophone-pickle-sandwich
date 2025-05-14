/// ---------------------------
/// physical-memory-manager.hpp
/// This file contains the definition of the Physical_Memory_Manager_Bitmap and Physical_Memory_Manager class.

#pragma once

#include <stdint.h>

#include "../../helpers/bitmap.hpp"

class Physical_Memory_Manager_Bitmap : public Bitmap<uint32_t>
{
    public:
        void Clear();
};

class Physical_Memory_Manager
{
    protected:
        Physical_Memory_Manager_Bitmap bitmap;
    public:
        void Initialize(uint32_t memory_size);

        uint32_t Allocate_Block();
        void Free_Block(uint32_t block_number);
        
        uint32_t Get_First_Free_Block(); 
};