/// -------------
/// kernel-main.c
/// @brief This is the C entry of the kernel.

#define cli() __asm__ volatile("cli")
#define sti() __asm__ volatile("sti")

#define get_esp(esp) __asm__ volatile("mov %%esp, %0" : "=r"(esp))

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <multiboot2.h>

#include <memory-spec.h>

#include "misc-drivers/vga-terminal.h"

// Wrappers
static void setup_misc(void);

static void setup_descriptors(void);

static void setup_bios32(void);

static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);

static void setup_drivers(void);

static void setup_scheduling(void);

void kernel_main(uint32_t addr, uint32_t magic)
{
    setup_misc();

    setup_descriptors();

    setup_bios32();

    setup_early_heap();

    setup_paging();

    setup_heap();

    setup_drivers();

    sti();

    setup_scheduling();

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
}

#include "descriptors/global-descriptor-table.h"
#include "descriptors/task-state-segment.h"
#include "descriptors/interrupt-descriptor-table.h"

void setup_descriptors(void)
{
    kernel_global_descriptor_table_init();
    kernel_global_descriptor_table_install();

    uint32_t esp;
    get_esp(esp);
    kernel_task_state_segment_int(5, 0x10, esp);

    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
}

#include "bios32/bios32.h"

void setup_bios32(void)
{
    kernel_bios32_init();
}

#include "memory-management/kheap.h"
void setup_early_heap(void)
{
    kheap_pre_init();
}

#include "memory-management/pmm.h"
#include "memory-management/paging.h"
void setup_paging(void)
{
    if(pmm_init(PHYSICAL_MEMORY_SIZE)){
        paging_init();
    }
}

void setup_heap(void)
{
    kheap_init();
}

#include "drivers/pit/pit.h"
void setup_drivers(void){
    pit_init(50);
}

#include <scheduling/scheduling.h>

void task1(void){
    while(true){

    }
    scheduling_exit_task();
}

void task2(void){
    while(true){
        
    }
    scheduling_exit_task();
}

void setup_scheduling(void){
    scheduling_init();
    scheduling_install();

    scheduling_create_task("ktask1", task1, true, false);
    scheduling_create_task("ktask2", task2, true, false);
}