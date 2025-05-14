/// ----------------
/// heap-manager.hpp
/// This file contains the definition of the Heap_Manager class.

#pragma once

#include "heap-structures.hpp"

class Paging_Manager;

class Heap_Manager
{
    protected:
        Paging_Manager* paging_manager;
        uint32_t placement_address;
    private:
        bool bInitialized;
        bool bPreInitialized;
    
        Heap_Array heap_array;

        bool is_supervisor;
        bool is_readonly;

        int Find_Smallest_Hole(uint32_t size, bool should_align = false);
        
        void Expand(uint32_t new_size);
        uint32_t Contract(uint32_t new_size);
        
        void* Allocate(uint32_t size, bool should_align = false);
        void _Free(uint32_t address);
    
        uint32_t start_address;
        uint32_t end_address;
        uint32_t max_end_address;
    
    public:
        uint32_t Get_Start_Address() const;
        uint32_t Get_End_Address() const;
        uint32_t Get_Max_End_Address() const;

        void Pre_Initialize(uint32_t end);
        void Initialize(uint32_t start_address, uint32_t end_address, uint32_t max_end_address, bool is_supervisor = false, bool is_readonly = false, Paging_Manager* paging_manager = nullptr);

        uint32_t Malloc(uint32_t size, bool should_align = false, uint32_t* physical_address = nullptr);
        void Free(uint32_t address);
};