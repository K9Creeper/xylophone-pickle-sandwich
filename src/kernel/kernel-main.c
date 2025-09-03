/// -------------------
/// @file kernel-main.c

#include <stdint.h>
#include <dbgprintf.h>
#include <kernel/util.h>
#include <kernel/data-structures/kernel-context.h>
#include <kernel/memory-management/heap-manager.h>
#include <kernel/memory-management/paging-manager.h>
#include <kernel/memory-management/physical-memory-manager.h>

#include "debug/serial-com1.h"
#include "../multiboot2/multiboot2.h"
#include "descriptors/global-descriptor-table.h"
#include "descriptors/interrupt-descriptor-table.h"
#include "interrupts/interrupts.h"
#include "bios32/bios32.h"
#include "syscalls/syscalls.h"
#include "drivers/pic-8259/pic-8259.h"
#include "drivers/programable-interval-timer/programable-interval-timer.h"
#include "drivers/vesa/vesa.h"

int system_interrupt_disable_counter = 0;
kernel_context_t *kernel_context = &(kernel_context_t){};

extern uint32_t __start_of_kernel_space;
extern uint32_t __start_of_kernel;
extern uint32_t __end_of_kernel;
extern uint32_t __kernel_heap_start;
extern uint32_t __kernel_heap_initial_end;
extern uint32_t __kernel_heap_max_end;

extern void page_fault_handler(void);
extern void general_protection_fault_handler(void);

void kernel_main(uint32_t magic, uint32_t addr)
{
    DISABLE_INTERRUPTS();
    if (!IS_VALID_MULTIBOOT2_MAGIC(magic)) return;

    /// -------------------
    /// Initialize Debug / Serial
    kernel_serial_com1_init();

    /// -------------------
    /// Initialize Descriptor Tables
    kernel_global_descriptor_table_init();
    kernel_global_descriptor_table_install();

    kernel_interrupt_descriptor_table_init();
    pic_8259_remap();
    kernel_interrupts_init();
    kernel_interrupt_descriptor_table_finalize();

    /// -------------------
    /// Get Physical Memory Info
    multiboot2_get_physical_memory_size(addr, &kernel_context->memory_info.reserved_memory);

    kernel_context->memory_info.useable_memory = kernel_context->memory_info.reserved_memory - ((uint32_t)(&__end_of_kernel) - (uint32_t)(&__start_of_kernel));

    /// -------------------
    /// Register Exception Handlers
    kernel_interrupts_add_service_handle(14, (interrupts_handle_t)page_fault_handler);
    kernel_interrupts_add_service_handle(13, (interrupts_handle_t)general_protection_fault_handler);

    /// -------------------
    /// Initialize System Calls
    syscalls_init();

    /// -------------------
    /// Initialize Memory Management
    heap_manager_bootstrap(&kernel_context->heap_manager, (uint32_t)(&__end_of_kernel));

    physical_memory_manager_init(
        &kernel_context->system_physical_memory_manager,
        kernel_context->memory_info.useable_memory,
        NULL,
        &kernel_context->heap_manager
    );

    page_directory_t *pd = heap_manager_malloc(&kernel_context->heap_manager, sizeof(page_directory_t), 1, NULL);

    paging_manager_init(
        &kernel_context->paging_manager,
        pd,
        &kernel_context->heap_manager,
        &kernel_context->system_physical_memory_manager,
        1
    );

    paging_manager_allocate_range(
        &kernel_context->paging_manager,
        (uint32_t)(&__start_of_kernel_space),
        kernel_context->heap_manager.placement_address + (1 * PAGE_SIZE),
        1,
        1
    );

    paging_manager_set_system_paging(&kernel_context->paging_manager, 0);
    paging_manager_enable(&kernel_context->paging_manager);

    heap_manager_init(
        &kernel_context->heap_manager,
        (uint32_t)(&__kernel_heap_start),
        (uint32_t)(&__kernel_heap_initial_end),
        (uint32_t)(&__kernel_heap_max_end),
        1, 0, 0,
        &kernel_context->paging_manager
    );

    /// -------------------
    /// Initialize BIOS32 / Other low-level services
    kernel_bios32_init();

    ENABLE_INTERRUPTS();
}