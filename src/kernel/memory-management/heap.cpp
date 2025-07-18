/// --------
/// heap.cpp
/// @breif This file contains the definitions to heap init related functions.

#include "heap.hpp"

#include "../../data-structures/memory-management/heap-manager.hpp"
#include "../../data-structures/memory-management/paging-manager.hpp"

namespace Kernel
{
    namespace Memory_Management
    {
        Heap_Manager heap_manager;
        extern Paging_Manager paging_manager;
    }
}

// linker.ld
extern uint32_t linkerld_endofkernel;

extern Heap_Manager* current_system_heap_manager;

void Kernel::Memory_Management::Heap_Pre_Init()
{
    uint32_t kernel_memory_end = reinterpret_cast<uint32_t>(&linkerld_endofkernel);

    current_system_heap_manager = &heap_manager;

    heap_manager.Pre_Initialize(kernel_memory_end);
}

void Kernel::Memory_Management::Heap_Init()
{
    heap_manager.Initialize(0xC0400000, 0xC0500000, 0xCFFFFF00, true, false, &paging_manager);
}