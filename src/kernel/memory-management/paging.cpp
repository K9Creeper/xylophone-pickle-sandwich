/// ----------
/// paging.cpp
/// @breif This file contains the definitions of Kernel paging initialzation functions.

#include "paging.hpp"

#include "../../data-structures/memory-management/paging-manager.hpp"
#include "../../data-structures/memory-management/heap-manager.hpp"

namespace Kernel
{
    namespace Memory_Management
    {
        Paging_Manager paging_manager;

        extern Heap_Manager heap_manager;
    }
}

void Kernel::Memory_Management::Paging_Init()
{
    paging_manager.Initialize(heap_manager.Malloc(sizeof(Page_Directory), true), &heap_manager, true);

    paging_manager.Post_Initialize();

    paging_manager.Allocate(0xC0000000, 0xC0000000 + 0x400000, true, true);

    paging_manager.Set_As_System_Paging(false);
    paging_manager.Enable();

    paging_manager.Identity_Allocate(0x0, 0x100000, true, true);
}