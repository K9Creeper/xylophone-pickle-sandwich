/// ----------------------
/// @file paging-manager.c

#include <kernel/memory-management/paging-manager.h>

#include <kernel/memory-management/heap-manager.h>
#include <kernel/memory-management/physical-memory-manager.h>

#include <memory.h>

// boot.s
extern page_directory_t *boot_page_directory;

static uint8_t is_enabled = 0;
static uint8_t is_pse_enabled = 1;

static uint32_t virtual_2_physical(page_directory_t *page_directory, uint32_t virtual_address);
static void allocate_page(page_directory_t *page_directory, uint32_t virtual_address, uint32_t frame, uint8_t is_kernel, uint8_t is_writeable, heap_manager_t *heap_manager, physical_memory_manager_t *physical_memory_manager);
static void free_page(page_directory_t *page_directory, uint32_t virtual_address, uint8_t should_free, physical_memory_manager_t *physical_memory_manager);

static void disable_pse_reg();

void paging_manager_init(paging_manager_t *paging_manager, page_directory_t *page_directory, struct heap_manager_s *heap_manager, struct physical_memory_manager_s *physical_memory_manager, uint8_t should_clear)
{
    if (paging_manager == NULL)
        return;

    paging_manager->heap_manager = heap_manager;
    paging_manager->physical_memory_manager = physical_memory_manager;

    paging_manager->page_directory = page_directory;

    if (should_clear)
    {
        memset((uint8_t *)paging_manager->page_directory, 0, sizeof(page_directory_t));
    }

    paging_manager->is_initialized = 1;
}

uint8_t paging_manager_set_system_paging(paging_manager_t *paging_manager, uint8_t is_physical)
{
    uint32_t t;
    if (!is_physical)
        t = virtual_2_physical(paging_manager->page_directory, (uint32_t)(paging_manager->page_directory));
    else
        t = (uint32_t)(paging_manager->page_directory);

    __asm__ volatile("mov %0, %%cr3" ::"r"(t));

    return 1;
}

uint8_t paging_manager_set_cr3(paging_manager_t *paging_manager, uint32_t cr3, uint8_t is_physical)
{
    uint32_t t;

    if (!is_physical)
    {
        t = virtual_2_physical(paging_manager->page_directory, cr3);
    }
    else
        t = cr3;

    __asm__ volatile("mov %0, %%cr3" ::"r"(t));

    return 1;
}

void paging_manager_enable(paging_manager_t *paging_manager)
{
    disable_pse_reg();

    uint32_t cr0;

    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));

    is_enabled = 1;
}

uint32_t paging_manager_get_physical_address(paging_manager_t *paging_manager, uint32_t virtual_address)
{
    return virtual_2_physical(paging_manager->page_directory, virtual_address);
}

void paging_manager_identity_allocate_single(paging_manager_t *paging_manager, uint32_t address, uint8_t is_kernel, uint8_t is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized || !paging_manager->heap_manager)
        return;
    allocate_page(paging_manager->page_directory, address, (address / PAGE_SIZE), is_kernel, is_writeable, paging_manager->heap_manager, paging_manager->physical_memory_manager);
}

void paging_manager_identity_allocate_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, uint8_t is_kernel, uint8_t is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized || !paging_manager->heap_manager)
        return;
    for (; start_address <= end_address; start_address += PAGE_SIZE)
    {
        allocate_page(paging_manager->page_directory, start_address, (start_address / PAGE_SIZE), is_kernel, is_writeable, paging_manager->heap_manager, paging_manager->physical_memory_manager);
    }
}

void paging_manager_allocate_single(paging_manager_t *paging_manager, uint32_t address, uint8_t is_kernel, uint8_t is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized || !paging_manager->heap_manager)
        return;
    allocate_page(paging_manager->page_directory, address, 0, is_kernel, is_writeable, paging_manager->heap_manager, paging_manager->physical_memory_manager);
}

void paging_manager_allocate_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, uint8_t is_kernel, uint8_t is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized || !paging_manager->heap_manager)
        return;

    for (; start_address <= end_address; start_address += PAGE_SIZE)
    {
        allocate_page(paging_manager->page_directory, start_address, 0, is_kernel, is_writeable, paging_manager->heap_manager, paging_manager->physical_memory_manager);
    }
}

void paging_manager_free_single(paging_manager_t *paging_manager, uint32_t address, uint8_t should_free)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    free_page(paging_manager->page_directory, address, should_free, paging_manager->physical_memory_manager);
}

void paging_manager_free_range(paging_manager_t *paging_manager, uint32_t start_address, uint32_t end_address, uint8_t should_free)
{
    if (!paging_manager || !paging_manager->is_initialized)
        return;
    for (; start_address <= end_address; start_address += PAGE_SIZE)
    {
        free_page(paging_manager->page_directory, start_address, should_free, paging_manager->physical_memory_manager);
    }
}

void paging_manager_allocate_to_physical(paging_manager_t *paging_manager, uint32_t virtual_address, uint32_t physical_address, uint8_t is_kernel, uint8_t is_writeable)
{
    if (!paging_manager || !paging_manager->is_initialized || !paging_manager->heap_manager)
        return;

    // Pass frame number (physical address >> 12)
    allocate_page(paging_manager->page_directory, virtual_address, (physical_address >> 12), is_kernel, is_writeable, paging_manager->heap_manager, paging_manager->physical_memory_manager);
}

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

static void allocate_page(page_directory_t *page_directory, uint32_t virtual_address, uint32_t frame, uint8_t is_kernel, uint8_t is_writeable, heap_manager_t *heap_manager, physical_memory_manager_t *physical_memory_manager)
{
    page_table_t *table = NULL;

    uint32_t page_dir_idx = virtual_address >> 22;
    uint32_t page_tbl_idx = (virtual_address >> 12) & 0x3ff;

    table = page_directory->ref_tables[page_dir_idx];

    if (!table)
    {
        table = heap_manager_malloc(heap_manager, sizeof(page_table_t), 1, NULL);

        memset((uint8_t *)(table), 0, sizeof(page_table_t));

        uint32_t t = virtual_2_physical(page_directory, (uint32_t)table);

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
        {
            t = physical_memory_manager_allocate_block(physical_memory_manager);
            if (!physical_memory_manager_is_valid_block(physical_memory_manager, t))
            {
                // Panic! PMM Block Allocation Failed
                physical_memory_manager->failed = 1;
                if (physical_memory_manager->failure != NULL)
                    physical_memory_manager->failure(physical_memory_manager);
                else
                    do
                    {
                        __asm__ volatile("hlt");
                    } while (1);
                return;
            }
        }

        table->pages[page_tbl_idx].frame = t;
        table->pages[page_tbl_idx].present = 1;
        table->pages[page_tbl_idx].rw = is_writeable ? 1 : 0;
        table->pages[page_tbl_idx].user = !is_kernel ? 1 : 0;
    }
}

static void free_page(page_directory_t *page_directory, uint32_t virtual_address, uint8_t should_free, physical_memory_manager_t *physical_memory_manager)
{
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
        physical_memory_manager_free_block(physical_memory_manager, table->pages[page_table_idx].frame);

    table->pages[page_table_idx].present = 0;
    table->pages[page_table_idx].frame = 0;
}

static void disable_pse_reg()
{
    if (!is_pse_enabled)
        return;

    uint32_t cr4;
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 &= 0xffffffef;
    __asm__ volatile("mov %0, %%cr4" ::"r"(cr4));

    is_pse_enabled = 0;
}