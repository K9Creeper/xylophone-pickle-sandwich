/// -------
/// pmm.cpp
/// @brief This file contains the definitions to phyical memory management init related functuins.

#include "pmm.hpp"

#include "../../data-structures/memory-management/physical-memory-manager.hpp"

// paging-manager.cpp
extern Physical_Memory_Manager system_physical_memory_manager;

void Kernel::Memory_Management::PMM_Init(uint32_t size){
    system_physical_memory_manager.Initialize(size);
}