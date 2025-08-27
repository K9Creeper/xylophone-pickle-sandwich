/// -------------------
/// @file kernel-main.c

#include <stdint.h>

#include <kernel/util.h>
int system_interrupt_disable_counter = 0;

#include <dbgprintf.h>
#include "debug/serial-com1.h"

#include "../multiboot2/multiboot2.h"

#include <kernel/data-structures/kernel-context.h>
kernel_context_t *kernel_context = &(kernel_context_t){};

#include "descriptors/global-descriptor-table.h"
#include "descriptors/interrupt-descriptor-table.h"

#include "interrupts/interrupts.h"

#include <kernel/memory-management/heap-manager.h>
#include <kernel/memory-management/paging-manager.h>
#include <kernel/memory-management/physical-memory-manager.h>

#include "syscalls/syscalls.h"

#include "drivers/pic-8259/pic-8259.h"
#include "drivers/programable-interval-timer/programable-interval-timer.h"

static void page_fault_handler(registers_t *r)
{
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    uint32_t present = r->error & 0x1;
    uint32_t rw = r->error & 0x2;
    uint32_t user = r->error & 0x4;
    uint32_t reserved = r->error & 0x8;
    uint32_t inst_fetch = r->error & 0x10;

    dbgprintf("Page Fault | Possible causes: [ ");
    if (!present)
        dbgprintf("Page not present ");
    if (rw)
        dbgprintf("Page is read only ");
    if (user)
        dbgprintf("Page is read only ");
    if (reserved)
        dbgprintf("Overwrote reserved bits ");
    if (inst_fetch)
        dbgprintf("Instruction fetch ");
    dbgprintf("] at 0x%X\n", faulting_address);
}

static void general_protection_fault_handler(registers_t *r)
{
    uint32_t error_code = r->error;

    uint32_t external = error_code & 0x1;     // 0 = internal, 1 = external event
    uint32_t table = (error_code >> 1) & 0x3; // 0 = GDT, 1 = IDT, 2 = LDT, ?? = ????
    uint32_t index = (error_code >> 3);       // Index of the descriptor in the table

    dbgprintf("General Protection Fault | Error code: 0x%X\n", error_code);
    dbgprintf("Details: [ ");
    if (external)
        dbgprintf("External event ");
    switch (table)
    {
    case 0:
        dbgprintf("GDT ");
        break;
    case 1:
        dbgprintf("IDT ");
        break;
    case 2:
        dbgprintf("LDT ");
        break;
    default:
        dbgprintf("Unknown table ");
        break;
    }
    dbgprintf("Index: %D ]\n", index);
    dbgprintf("EIP at fault: 0x%X\n", r->eip);
}

void kernel_main(uint32_t magic, uint32_t addr)
{
    DISABLE_INTERRUPTS();
    if (!IS_VALID_MULTIBOOT2_MAGIC(magic))
        return;

    kernel_serial_com1_init();

    kernel_global_descriptor_table_init();
    kernel_global_descriptor_table_install();

    kernel_interrupt_descriptor_table_init();
    pic_8259_remap();
    kernel_interrupts_init();
    kernel_interrupt_descriptor_table_finalize();

    multiboot2_get_physical_memory_size(addr, &kernel_context->memory_info.useable_memory);

    kernel_interrupts_add_service_handle(14, (interrupts_handle_t)page_fault_handler);
    kernel_interrupts_add_service_handle(13, (interrupts_handle_t)general_protection_fault_handler);

    syscalls_init();

    {
        // linker.ld
        extern uint32_t __start_of_kernel_space;
        extern uint32_t __start_of_kernel;
        extern uint32_t __end_of_kernel;
        extern uint32_t __kernel_heap_start;
        extern uint32_t __kernel_heap_initial_end;
        extern uint32_t __kernel_heap_max_end;

        heap_manager_bootstrap(&kernel_context->heap_manager, (uint32_t)(&__end_of_kernel));

        physical_memory_manager_init(&kernel_context->system_physical_memory_manager, kernel_context->memory_info.useable_memory, NULL, &kernel_context->heap_manager);

        page_directory_t *pd = heap_manager_malloc(&kernel_context->heap_manager, sizeof(page_directory_t), 1, NULL);

        paging_manager_init(
            &kernel_context->paging_manager,
            pd,
            &kernel_context->heap_manager,
            &kernel_context->system_physical_memory_manager,
            1);

        paging_manager_allocate_range(&kernel_context->paging_manager, (uint32_t)(&__start_of_kernel_space), kernel_context->heap_manager.placement_address + (1 * PAGE_SIZE), 1, 1);

        paging_manager_set_system_paging(&kernel_context->paging_manager, 0);
        paging_manager_enable(&kernel_context->paging_manager);

        heap_manager_init(&kernel_context->heap_manager, (uint32_t)(&__kernel_heap_start), (uint32_t)(&__kernel_heap_initial_end), (uint32_t)(&__kernel_heap_max_end), 1, 0, 0, &kernel_context->paging_manager);
    }

    ENABLE_INTERRUPTS();
}

void *kernel_malloc(uint32_t size)
{
    return heap_manager_malloc(&kernel_context->heap_manager, size, 0, NULL);
}

void kernel_free(void *ptr)
{
    heap_manager_free(&kernel_context->heap_manager, (uint32_t)ptr);
}

void *kernel_amalloc(uint32_t size)
{
    return heap_manager_malloc(&kernel_context->heap_manager, size, 1, NULL);
}

void *kernel_calloc(uint32_t n, uint32_t size)
{
    uint32_t total = n * size;
    void *ptr = kernel_amalloc(total);
    if (ptr)
        memset(ptr, 0, total);
    return ptr;
}

// NOTE: a really poor implementation (not utilizing core heap functionality / data)
void *kernel_realloc(void *ptr, uint32_t size)
{
    if (!ptr)
        return kernel_amalloc(size);

    if (size == 0)
    {
        kernel_free(ptr);
        return NULL;
    }

    void *new_ptr = kernel_amalloc(size);
    if (!new_ptr)
        return NULL;

    memcpy(new_ptr, ptr, size);
    kernel_free(ptr);
    return new_ptr;
}