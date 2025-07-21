/// ----------------
/// paging-manager.c
/// @brief This file contains the definitions of the paging_manager structure methods...and managing overall system paging.

#include <memory-management/paging-manager.h>

#include <memory.h>

#include <memory-management/physical-memory-manager.h>
#include <memory-management/heap-manager.h>

#include <stdio.h>

// boot.s
extern page_directory_t *boot_page_directory;

// heap-manager.c
extern heap_manager_t *current_system_heap_manager;

static bool is_enabled = false;
static bool is_pse_enabled = true;

physical_memory_manager_t system_physical_memory_manager;

page_directory_t *current_system_page_directory = NULL;
paging_manager_t *current_system_paging_manager = NULL;

static uint32_t virtual_2_physical(page_directory_t *page_directory, uint32_t virtual_address)
{
    if (!is_enabled)
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

    page_table_t *table = page_directory->ref_tables[page_dir_idx];
    if (!table->pages[page_table_idx].present)
    {
        return 0;
    }

    return (table->pages[page_table_idx].frame << 12) + page_frame_offset;
}

static void disable_pse_reg()
{
    if (!is_pse_enabled)
        return;

    uint32_t cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 &= 0xffffffef;
    __asm__ volatile("mov %0, %%cr4" ::"r"(cr4));

    is_pse_enabled = false;
}

static void swap_system_page_directory(uint32_t cr3, bool is_physical)
{
    if (!cr3)
        return;

    uint32_t t;

    if (!is_physical)
    {
        t = virtual_2_physical(boot_page_directory, cr3);
    }
    else
        t = cr3;

    __asm__ volatile("mov %0, %%cr3" ::"r"(t));
}

static void allocate_page(page_directory_t *page_directory, uint32_t virtual_address, uint32_t frame, bool is_kernel, bool is_writeable)
{
    if (!page_directory)
        return;

    page_table_t *table = NULL;

    uint32_t page_dir_idx = virtual_address >> 22;
    uint32_t page_tbl_idx = (virtual_address >> 12) & 0x3ff;

    table = page_directory->ref_tables[page_dir_idx];

    if (!table)
    {
        table = heap_manager_malloc(current_system_heap_manager, sizeof(page_table_t), true, NULL);

        memset((uint8_t*)(table), 0, sizeof(page_table_t));

        uint32_t t = virtual_2_physical(current_system_page_directory, (uint32_t)table);

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
        else{
            t = physical_memory_manager_allocate_block(&system_physical_memory_manager);
            if(!physical_memory_manager_is_valid_block(&system_physical_memory_manager, t))
                {
                    // Panic! PMM Block Allocation Failed
                    system_physical_memory_manager.failed = true;
                    printf("PMM Failed Allocating Block %d\n", t);
                    return;
                }
        }

        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = is_writeable ? 1 : 0;
        table->pages[page_tbl_idx].user = !is_kernel ? 1 : 0;
    }
}

static void free_page(page_directory_t *page_directory, uint32_t virtual_address, bool should_free)
{
    if (page_directory == boot_page_directory)
        return;

    uint32_t page_dir_idx = virtual_address >> 22;
    uint32_t page_table_idx = (virtual_address >> 12) & 0x3ff;

    if (!page_directory->ref_tables[page_dir_idx])
    {
        return;
    }

    page_table_t *table = page_directory->ref_tables[page_dir_idx];
    if (!table->pages[page_table_idx].present)
    {
        return;
    }

    if (should_free)
        physical_memory_manager_free_block(&system_physical_memory_manager, table->pages[page_table_idx].frame);

    table->pages[page_table_idx].present = 0;
    table->pages[page_table_idx].frame = 0;
}

void paging_manager_init(paging_manager_t *paging_manager, page_directory_t *page_directory, struct heap_manager_s *heap_manager, bool should_clear)
{
    if (!current_system_page_directory)
        current_system_page_directory = boot_page_directory;

    if (!paging_manager || !page_directory || !heap_manager || !system_physical_memory_manager.is_initialized)
        return;

    paging_manager->page_directory = page_directory;

    if (should_clear)
        memset((uint8_t*)paging_manager->page_directory, 0, sizeof(page_directory_t));

    paging_manager->heap_manager = heap_manager;

    paging_manager->is_initialized = true;
    paging_manager->is_enabled = false;
}

void paging_manager_swap_system_page_directory(paging_manager_t* paging_manager, uint32_t cr3, bool is_physical){
    if (!paging_manager || !paging_manager->is_initialized)
        return;
        
     uint32_t t;

    if (cr3)
    {
        if (!is_physical)
        {
            t = virtual_2_physical(boot_page_directory, cr3);
        }
        else
            t = cr3;
    }
    else
    {
        if (!is_physical)
            t = virtual_2_physical(boot_page_directory, (uint32_t)paging_manager->page_directory);
        else
            t = (uint32_t)(paging_manager->page_directory);
    }
    
    __asm__ volatile("mov %0, %%cr3" ::"r"(t));

    current_system_paging_manager = paging_manager;
    current_system_page_directory = paging_manager->page_directory;
    current_system_heap_manager = paging_manager->heap_manager;
}

void paging_manager_set_as_system_paging(paging_manager_t *paging_manager, bool is_physical)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;

    uint32_t t;
    if (!is_physical)
        t = virtual_2_physical(boot_page_directory, (uint32_t)(paging_manager->page_directory));
    else
        t = (uint32_t)(paging_manager->page_directory);

    __asm__ volatile("mov %0, %%cr3" ::"r"(t));

    current_system_paging_manager = paging_manager;
    current_system_page_directory = paging_manager->page_directory;
    current_system_heap_manager = paging_manager->heap_manager;
}

void paging_manager_swap_page_directory(paging_manager_t *paging_manager, page_directory_t *page_directory)
{
    if (!page_directory)
        return;

    paging_manager->page_directory = page_directory;
}

uint32_t paging_manager_get_physical_address(paging_manager_t *paging_manager, uint32_t virtual_address)
{
    return virtual_2_physical(paging_manager->page_directory, virtual_address);
}

void paging_manager_enable(paging_manager_t *paging_manager)
{
    if (paging_manager->is_enabled || !paging_manager->is_initialized)
        return;

    disable_pse_reg();

    uint32_t cr0;

    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));

    is_enabled = true;
    paging_manager->is_enabled = true;
}

void paging_manager_identity_allocate_single(paging_manager_t *paging_manager, uint32_t address, bool is_kernel, bool is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    allocate_page(paging_manager->page_directory, address, (address / PAGE_SIZE), is_kernel, is_writeable);
}

void paging_manager_identity_allocate_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    for (; start_address < end_address + PAGE_SIZE; start_address += PAGE_SIZE)
    {
        paging_manager_identity_allocate_single(paging_manager, start_address, is_kernel, is_writeable);
    }
}

void paging_manager_allocate_single(paging_manager_t *paging_manager, uint32_t address, bool is_kernel, bool is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    allocate_page(paging_manager->page_directory, address, 0, is_kernel, is_writeable);
}

void paging_manager_allocate_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, bool is_kernel, bool is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;

    for (; start_address < end_address + PAGE_SIZE; start_address += PAGE_SIZE)
    {
        paging_manager_allocate_single(paging_manager, start_address, is_kernel, is_writeable);
    }
}

void paging_manager_free_single(paging_manager_t *paging_manager, uint32_t address, bool should_free)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    free_page(paging_manager->page_directory, address, should_free);
}

void paging_manager_free_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, bool should_free)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    for (; start_address < end_address + PAGE_SIZE; start_address += PAGE_SIZE)
    {
        paging_manager_free_single(paging_manager, start_address, should_free);
    }
}