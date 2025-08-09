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

#include "drivers/vga-terminal/vga-terminal.h"

#include <kernel/util.h>

int system_interrupt_disable_counter = 0;

// Wrappers
static void setup_misc(void);

static void setup_descriptors(void);

static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);

static void setup_fault_handlers(void);

static int setup_vesa(void);

static void setup_drivers(void);

static void setup_scheduling(void);
static void finish_scheduling(void);

static void enable_keyboard_input(void);

void kernel_main(uint32_t addr, uint32_t magic)
{
    DISABLE_INTERRUPTS();

    setup_misc();

    setup_descriptors();

    setup_fault_handlers();

    setup_early_heap();

    setup_paging();

    setup_heap();

    setup_drivers();

    ENABLE_INTERRUPTS();

    enable_keyboard_input();

    // allow user input
    
    //setup_vesa();

    DISABLE_INTERRUPTS();

    /*
    setup_scheduling();

    finish_scheduling();
    */

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
    vga_terminal_show_cursor(false);
}

#include "descriptors/global-descriptor-table.h"
#include "bios32/bios32.h"
#include "descriptors/task-state-segment.h"
#include "descriptors/interrupt-descriptor-table.h"

void setup_descriptors(void)
{
    kernel_global_descriptor_table_init();
    bios32_init();
    kernel_global_descriptor_table_install();

    uint32_t esp;
    get_esp(esp);
    kernel_task_state_segment_int(5, 0x10, esp);

    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
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
     // (16 * # )MB
    if (pmm_init(0x1000000 * 8U))
    {
        paging_init();
    }
}

void setup_heap(void)
{
    kheap_init();
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
}

#include "drivers/pit/pit.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/syscall/syscall.h"
void setup_drivers(void)
{
    syscall_init();
    keyboard_init();
    pit_init(1000);
}

#include <scheduling/scheduling.h>
#include "kthread/kthread.h"
void setup_scheduling(void)
{
    scheduling_init();
}

void finish_scheduling(void)
{
    //pit_add_handle((pit_handle_t)scheduling_schedule);
}


static void vga_terminal_keyboard_input_handle(keyboard_key_t keyboard_key, const keyboard_map_t keyboard_map)
{
    if (keyboard_key.value != '\0')
    {
        vga_terminal_write(&keyboard_key.value, 1);
    }
}

void enable_keyboard_input(void){
    vga_terminal_show_cursor(true);
    keyboard_add_input_handle((keyboard_input_handle_t)vga_terminal_keyboard_input_handle);
}

#include "drivers/vesa/vesa.h"
int setup_vesa(void){
    if(vesa_init())
        return 1;
    
    vga_terminal_destroy();

    if(vesa_set_specs(1024, 768))
    {
        return 2;
    }

    return 0;
}