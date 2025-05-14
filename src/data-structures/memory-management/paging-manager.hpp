/// ------------------
/// paging-manager.hpp
/// This file contains the definition of the Paging_Manager class.

#pragma once

#include "paging-structures.hpp"

class Heap_Manager;

class Paging_Manager{
    protected:
        Heap_Manager* heap_manager;
        Page_Directory* page_directory;
    private:
        bool bInitialized;
        bool bEnabled;
        
    public:
        void Initialize(Page_Directory* page_directory, Heap_Manager* heap_manager, bool should_clear = true);
        
        // Only used for pre-initialization IN THE KERNEL, never after...only ran one time THROUGHOUT FULL RUNTIME.
        void Post_Initialize();

        // Getters & Setters
        bool Is_Enabled() const;
        bool Is_Initialized() const;

        void Swap_Heap_Manager(Heap_Manager* heap_manager);
        Heap_Manager* Get_Heap_Manager() const;

        static void Swap_System_Page_Directory(uint32_t cr3, bool is_physical = false);
        
        void Set_As_System_Paging(bool is_physical = false);
        void Swap_Page_Directory(Page_Directory* page_directory);
        Page_Directory* Get_Page_Directory() const;

        uint32_t Get_Physical_Address(uint32_t virtual_address) const;

        // Do(ers)
        void Enable();

        static void Allocate_Page(Page_Directory* page_directory, uint32_t address, uint32_t frame, bool is_kernel = false, bool is_writeable = false);

        void Identity_Allocate(uint32_t address, bool is_kernel = false, bool is_writeable = false);
        void Identity_Allocate(uint32_t start_address, uint32_t end_address, bool is_kernel = false, bool is_writeable = false);

        void Allocate(uint32_t address, bool is_kernel = false, bool is_writeable = false);
        void Allocate(uint32_t start_address, uint32_t end_address, bool is_kernel = false, bool is_writeable = false);

        static void Free_Page(Page_Directory* page_directory, uint32_t address, bool should_free);

        void Free(uint32_t address, bool should_free);
        void Free(uint32_t start_address, uint32_t end_address, bool should_free);
};