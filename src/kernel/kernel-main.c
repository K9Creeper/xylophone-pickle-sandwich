/// -------------
/// kernel-main.c
/// @brief This is the C entry of the kernel.

#define get_esp(esp) __asm__ volatile("mov %%esp, %0" : "=r"(esp))

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/stdlib.h>
#include <memory.h>
#include <multiboot2.h>

#include <memory-spec.h>

#include "misc-drivers/vga-terminal.h"

#include <kernel/util.h>

int system_interrupt_disable_counter = 0;

// Wrappers
static void setup_misc(void);

static void setup_descriptors(void);

static void setup_bios32(void);

static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);

void setup_fault_handlers(void);

static void setup_drivers(void);

static void setup_scheduling(void);
static void finish_scheduling(void);

static int32_t _syscall_1(uint32_t syscall, uint32_t arg0)
{
    int32_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0));
    return ret;
}

void kernel_main(uint32_t addr, uint32_t magic)
{
    DISABLE_INTERRUPTS();

    setup_misc();
    
    setup_descriptors();

    setup_fault_handlers();

    setup_bios32();

    setup_early_heap();

    setup_paging();

    setup_heap();

    setup_drivers();

    setup_scheduling();

    finish_scheduling();

    vga_terminal_write_string("Hello!\nI hope you are having a wonderful day\n");

    ENABLE_INTERRUPTS();

halt:
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

#include "misc-drivers/serial-com1.h"

void setup_misc(void)
{
    kernel_misc_drivers_serial_com1_init();

    vga_terminal_init(DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS, VGA_TERMINAL_COLOR_LIGHT_BLUE, VGA_TERMINAL_COLOR_DARK_GREY);
    vga_terminal_write_string("[Kernel Main] Setup VGA Terminal\n");
}

#include "descriptors/global-descriptor-table.h"
#include "descriptors/task-state-segment.h"
#include "descriptors/interrupt-descriptor-table.h"

void setup_descriptors(void)
{
    kernel_global_descriptor_table_init();
    vga_terminal_write_string("[Kernel Main] Init GDT\n");
    kernel_global_descriptor_table_install();
    vga_terminal_write_string("[Kernel Main] Installed GDT\n");

    uint32_t esp;
    get_esp(esp);
    kernel_task_state_segment_int(5, 0x10, esp);
    vga_terminal_write_string("[Kernel Main] Init (Basic) TSS\n");

    kernel_interrupt_descriptor_table_init();
    vga_terminal_write_string("[Kernel Main] Init IDT\n");
    kernel_interrupt_descriptor_table_install();
    vga_terminal_write_string("[Kernel Main] Installed IDT\n");
}

#include "bios32/bios32.h"

void setup_bios32(void)
{
    kernel_bios32_init();
    vga_terminal_write_string("[Kernel Main] Init BIOS calls\n");
}

#include "memory-management/kheap.h"
void setup_early_heap(void)
{
    kheap_pre_init();
    vga_terminal_write_string("[Kernel Main] Setup early KHeap\n");
}

#include "memory-management/pmm.h"
#include "memory-management/paging.h"
void setup_paging(void)
{
    if (pmm_init(PHYSICAL_MEMORY_SIZE))
    {
        vga_terminal_write_string("[Kernel Main] Init PMM\n");
        paging_init();
        vga_terminal_write_string("[Kernel Main] Init Paging\n");
    }
}

void setup_heap(void)
{
    kheap_init();
    vga_terminal_write_string("[Kernel Main] Finished Init KHeap\n");
}

#include "interrupts/interrupt-service.h"
static void page_fault_handler(registers_t *r)
{
    uint32_t faulting_address;

    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    // The error code gives us details of what happened
    uint32_t present = r->error & 0x1;
    uint32_t rw = r->error & 0x2;
    uint32_t user = r->error & 0x4;
    uint32_t reserved = r->error & 0x8;
    uint32_t inst_fetch = r->error & 0x10;

    printf("Page Fault | Possible causes: [ ");
    if (!present)
        printf("Page not present ");
    if (rw)
        printf("Page is read only ");
    if (user)
        printf("Page is read only ");
    if (reserved)
        printf("Overwrote reserved bits ");
    if (inst_fetch)
        printf("Instruction fetch ");
    printf("] at ");
    printf("0x%X\n", faulting_address);
}

void setup_fault_handlers(void)
{
    kernel_interrupt_service_set_fault_handle(14, page_fault_handler);
    
    vga_terminal_write_string("[Kernel Main] Setup Fault Handlers\n");
}

#include "drivers/pit/pit.h"
#include "drivers/syscall/syscall.h"
void setup_drivers(void)
{
    syscall_init();
    pit_init(1000);
    
    vga_terminal_write_string("[Kernel Main] Setup Drivers\n");
}

void idle_task1(void)
{
    
}

void idle_task2(void)
{
    while (true)
    {
        printf("idle2 is running...\n");
    }
}

#include <scheduling/scheduling.h>
#include "kthread/kthread.h"
void setup_scheduling(void)
{
    scheduling_init();
    vga_terminal_write_string("[Kernel Main] Init Scheduling\n");

    kthread_register(idle_task1, "idletask1");
    kthread_register(idle_task2, "idletask2");

    vga_terminal_write_string("[Kernel Main] Added Kernel Threads\n");
}

void finish_scheduling(void)
{
    kthread_start("idletask1", 0, NULL);
    kthread_start("idletask2", 0, NULL);
    
    pit_add_handle((pit_handle_t)scheduling_schedule);
    vga_terminal_write_string("[Kernel Main] Finished Scheduling\n");
}