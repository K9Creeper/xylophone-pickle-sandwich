/// ------------------
/// paging-manager.cpp
/// This file contains the definitions of the Paging_Manager class methods...and managing overall system paging.

#include "paging-manager.hpp"

#include "heap-manager.hpp"

#include "physical-memory-manager.hpp"

#include "../../helpers/memory.h"

// boot.s
extern "C" Page_Directory *boot_page_directory;

Physical_Memory_Manager system_physical_memory_manager;

Page_Directory *current_system_page_directory = boot_page_directory;
Paging_Manager *current_system_paging_manager = nullptr;
Heap_Manager *current_system_heap_manager = nullptr;

static bool bEnabled = false;
static bool bPSEEnabled = true;

static uint32_t Virtual_2_Physical(Page_Directory *page_directory, uint32_t virtual_address)
{
    
    if (!bEnabled)
    {
        return (virtual_address - 0xC0000000);
    }

    if (!page_directory)
    {
        return 0;
    }

    uint32_t page_dir_idx = virtual_address >> 22;
    uint32_t page_table_idx = (virtual_address >> 12) & 0x3ff;
    uint32_t page_frame_offset = (virtual_address & 0xfff);

    if (!page_directory->ref_tables[page_dir_idx])
    {
        return 0;
    }

    Page_Table *table = page_directory->ref_tables[page_dir_idx];
    if (!table->pages[page_table_idx].present)
    {
        return 0;
    }

    return (table->pages[page_table_idx].frame << 12) + page_frame_offset;
}

static void DisablePSEReg()
{
    if (!bPSEEnabled)
        return;

    uint32_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 &= 0xffffffef;
    asm volatile("mov %0, %%cr4" ::"r"(cr4));

    bPSEEnabled = false;
}

void Paging_Manager::Initialize(Page_Directory *page_directory, Heap_Manager *heap_manager, bool should_clear)
{
    if (!page_directory || !heap_manager)
        return;

    this->page_directory = page_directory;

    if (should_clear)
        memset(reinterpret_cast<uint8_t *>(this->page_directory), 0, sizeof(Page_Directory));

    this->heap_manager = heap_manager;

    this->bInitialized = true;
}

void Paging_Manager::Post_Initialize()
{
    current_system_heap_manager = this->heap_manager;
    current_system_paging_manager = this;
}

bool Paging_Manager::Is_Enabled() const
{
    return bEnabled;
}

bool Paging_Manager::Is_Initialized() const
{
    return bInitialized;
}

void Paging_Manager::Swap_Heap_Manager(Heap_Manager *heap_manager)
{
    this->heap_manager = heap_manager;
}

Heap_Manager *Paging_Manager::Get_Heap_Manager() const
{
    return this->heap_manager;
}

// Never really used, but useful for just in case.
void Paging_Manager::Swap_System_Page_Directory(uint32_t cr3, bool is_physical = false)
{
    if (!cr3)
        return;

    uint32_t t;

    if (!is_physical)
    {
        t = Virtual_2_Physical(boot_page_directory, cr3);
    }
    else
        t = cr3;

    asm volatile("mov %0, %%cr3" ::"r"(t));
}

void Paging_Manager::Set_As_System_Paging(bool is_physical = false)
{
    uint32_t t;

    if (!is_physical)
        t = Virtual_2_Physical(boot_page_directory, reinterpret_cast<uint32_t>(this->page_directory));
    else
        t = reinterpret_cast<uint32_t>(this->page_directory);

    asm volatile("mov %0, %%cr3" ::"r"(t));

    current_system_paging_manager = this;
    current_system_page_directory = this->page_directory;
    current_system_heap_manager = this->heap_manager;
}

void Paging_Manager::Swap_Page_Directory(Page_Directory *page_directory)
{
    if (!page_directory)
        return;

    this->page_directory = page_directory;
}

Page_Directory *Paging_Manager::Get_Page_Directory() const
{
    return this->page_directory;
}

uint32_t Paging_Manager::Get_Physical_Address(uint32_t virtual_address) const
{
    return Virtual_2_Physical(this->page_directory, virtual_address);
}

void Paging_Manager::Enable()
{
    if (bEnabled)
        return;

    DisablePSEReg();

    uint32_t cr0;

    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" ::"r"(cr0));

    bEnabled = true;
}

void Paging_Manager::Allocate_Page(Page_Directory *page_directory, uint32_t virtual_address, uint32_t frame, bool is_kernel, bool is_writeable)
{
    if (!page_directory)
        return;

    Page_Table *table = nullptr;

    uint32_t page_dir_idx = virtual_address >> 22;
    uint32_t page_tbl_idx = (virtual_address >> 12) & 0x3ff;

    table = page_directory->ref_tables[page_dir_idx];

    if (!table)
    {
        table = current_system_heap_manager->Malloc(sizeof(Page_Table), true);

        memset(reinterpret_cast<uint8_t *>(table), 0, sizeof(Page_Table));

        uint32_t t = Virtual_2_Physical(current_system_page_directory, table);

        page_directory->tables[page_dir_idx].frame = t >> 12;
        page_directory->tables[page_dir_idx].present = 1;
        page_directory->tables[page_dir_idx].rw = is_writeable ? 1 : 0;
        page_directory->tables[page_dir_idx].user = !is_kernel ? 1 : 0;
        page_directory->tables[page_dir_idx].page_size = 0;

        page_directory->ref_tables[page_dir_idx] = table;
    }

    if (!table->pages[page_tbl_idx].present)
    {
        uint32_t t;
        if (frame)
            t = frame;
        else
            t = system_physical_memory_manager.Allocate_Block();

        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = is_writeable ? 1 : 0;
        table->pages[page_tbl_idx].user = !is_kernel ? 1 : 0;
    }
}

void Paging_Manager::Identity_Allocate(uint32_t address, bool is_kernel, bool is_writeable)
{
    Allocate_Page(this->page_directory, address, (address / 0x1000), is_kernel, is_writeable);
}

void Paging_Manager::Identity_Allocate(uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable)
{
    for (; start_address < end_address + 0x1000; start_address += 0x1000)
    {
        Identity_Allocate(start_address, is_kernel, is_writeable);
    }
}

void Paging_Manager::Allocate(uint32_t address, bool is_kernel, bool is_writeable)
{
    Allocate_Page(this->page_directory, address, 0, is_kernel, is_writeable);
}

void Paging_Manager::Allocate(uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable)
{
    for (; start_address < end_address; start_address += 0x1000)
    {
        Allocate(start_address, is_kernel, is_writeable);
    }
}

void Paging_Manager::Free_Page(Page_Directory *page_directory, uint32_t address, bool should_free)
{
    if (page_directory == boot_page_directory)
        return;

    uint32_t page_dir_idx = address >> 22;
    uint32_t page_table_idx = (address >> 12) & 0x3ff;

    if (!page_directory->ref_tables[page_dir_idx])
    {
        return;
    }

    Page_Table *table = page_directory->ref_tables[page_dir_idx];
    if (!table->pages[page_table_idx].present)
    {
        return;
    }

    if (should_free)
        system_physical_memory_manager.Free_Block(table->pages[page_table_idx].frame);

    table->pages[page_table_idx].present = 0;
    table->pages[page_table_idx].frame = 0;
}

void Paging_Manager::Free(uint32_t address, bool should_free)
{
    Free_Page(this->page_directory, address, should_free);
}

void Paging_Manager::Free(uint32_t start_address, uint32_t end_address, bool should_free)
{
    for (; start_address <= end_address; start_address += 0x1000)
    {
        Free(start_address, should_free);
    }
}