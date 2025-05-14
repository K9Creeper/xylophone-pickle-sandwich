/// ---------------------------
/// physical-memory-manager.cpp
/// This file contains the definition of the Physical_Memory_Manager_Bitmap and Physical_Memory_Manager class methods.

#include "physical-memory-manager.hpp"

#include "heap-manager.hpp"

#include "../../helpers/memory.h"

extern Heap_Manager kernel_heap_manager;

void Physical_Memory_Manager_Bitmap::Clear()
{
    memset(reinterpret_cast<uint8_t *>(array), 0, memory_size);
}

void Physical_Memory_Manager::Initialize(uint32_t memory_size)
{
    bitmap.Create();

    const int count = memory_size / 0x1000;
    const int size = (count / bitmap.Get_Bytes_Per_Entry());

    bitmap.Place(reinterpret_cast<void *>(kernel_heap_manager.Malloc(size)), size);

    bitmap.Clear();
}

uint32_t Physical_Memory_Manager::Allocate_Block(){
    uint32_t free_block = Get_First_Free_Block();

    uint32_t index = bitmap.Index_From_Bit(free_block);
    uint32_t offset = bitmap.Offset_From_Bit(free_block);

    bitmap[index] |= (0x1 << offset);

    return free_block;
}

void Physical_Memory_Manager::Free_Block(uint32_t block_number){
    uint32_t index = bitmap.Index_From_Bit(block_number);
    uint32_t offset = bitmap.Offset_From_Bit(block_number);

    bitmap[index] &= ~(0x1 << offset);
}

uint32_t Physical_Memory_Manager::Get_First_Free_Block(){
    for(uint32_t i = 0; i < bitmap.Get_Count(); i++) {
        if(!((bitmap[bitmap.Index_From_Bit(i)] >> (bitmap.Offset_From_Bit(i))) & 0x1))
            return i;
    }
    return static_cast<uint32_t>(-1);
}