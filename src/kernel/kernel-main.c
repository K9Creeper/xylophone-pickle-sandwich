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
#include <data-structures/kernel/kernel-context.h>
#include <kernel/util.h>

#include "../terminal/terminal.h"

int system_interrupt_disable_counter = 0;

kernel_context_t *kernel_context = &(kernel_context_t){
    .physical_memory_size = 0x1000000 * 8U,
    .video_state = {
        true,
        VGA_TERMINAL_WIDTH,
        VGA_TERMINAL_HEIGHT,
        0, 0,
        DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS}};

// Syscall helpers
static int32_t _syscall_0(uint32_t syscall)
{
    int32_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
    return ret;
}

static int32_t _syscall_1(uint32_t syscall, uint32_t arg0)
{
    int32_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0));
    return ret;
}

void syscall_print(const char *str);

// Setup function declarations
static void setup_misc(void);
static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);
static void setup_gdt(void);
static void setup_idt(void);
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
    setup_gdt();
    setup_idt();
    setup_early_heap();
    setup_paging();
    setup_heap();
    setup_drivers();
    setup_fault_handlers();

    ENABLE_INTERRUPTS();

    enable_keyboard_input();

    char buffer[4];
    while (true)
    {
        vga_terminal_write_string("Enter a graphical interface (y/n): ");

        while (!terminal_get_input(buffer, 128))
        {
            __asm__ volatile("hlt");
        }

        if (strcmp(buffer, "y") == 0)
        {
            if (setup_vesa())
            {
                vga_terminal_write_string("Graphical interface is unavaliable\n");
                continue;
            }
            break;
        }
        else if (strcmp(buffer, "n") == 0)
        {
            break;
        }
    }

    DISABLE_INTERRUPTS();
    setup_scheduling();
    finish_scheduling();
    ENABLE_INTERRUPTS();

halt:
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

// ------------------------------------------------------------
// Setup: Misc
#include "misc-drivers/serial-com1.h"

void setup_misc(void)
{
    kernel_misc_drivers_serial_com1_init();
    vga_terminal_init(DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS, VGA_TERMINAL_COLOR_MAGENTA, VGA_TERMINAL_COLOR_LIGHT_GREY);
    vga_terminal_show_cursor(false);
}

// ------------------------------------------------------------
// Setup: Descriptors
#include "descriptors/global-descriptor-table.h"
#include "bios32/bios32.h"
#include "descriptors/task-state-segment.h"
#include "descriptors/interrupt-descriptor-table.h"
static void setup_gdt(void)
{
    kernel_global_descriptor_table_init();
    bios32_init();
    kernel_global_descriptor_table_install();

    uint32_t esp;
    get_esp(esp);
    kernel_task_state_segment_int(5, 0x10, esp);
}
static void setup_idt(void)
{
    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
}

// ------------------------------------------------------------
// Setup: Memory
#include "memory-management/kheap.h"

void setup_early_heap(void)
{
    kheap_pre_init();
}

#include "memory-management/pmm.h"
#include "memory-management/paging.h"

void setup_paging(void)
{
    if (pmm_init(kernel_context->physical_memory_size))
    {
        paging_init();
    }
}

void setup_heap(void)
{
    kheap_init();
}

// ------------------------------------------------------------
// Setup: Fault Handlers
#include "interrupts/interrupt-service.h"

static void page_fault_handler(registers_t *r)
{
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

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
    printf("] at 0x%X\n", faulting_address);
}

void setup_fault_handlers(void)
{
    kernel_interrupt_service_set_fault_handle(14, page_fault_handler);
}

// ------------------------------------------------------------
// Setup: Drivers
#include "drivers/pit/pit.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/mouse/mouse.h"
#include "drivers/syscalls/syscalls.h"
static void test(registers_t *d, uint32_t t)
{
    if (t % pit_get_hz() == 0)
        vga_terminal_write_string("Tick %d\n", t);
}

void setup_drivers(void)
{
    syscalls_init();
    keyboard_init();
    mouse_init();
    pit_init(1000);

    // pit_add_handle((pit_handle_t)test);
}

// ------------------------------------------------------------
// Setup: Scheduling
#include <scheduling/scheduling.h>
#include "kthread/kthread.h"

// kthread/thread/**
extern void kthread_graphics(void);
extern void kthread_task_cleaner(void);

void kthread_idle(void)
{
    while (true)
    {
        scheduling_yield();
    }
}

void kthread_aids(void)
{
    for (int i = 0; i < 10; i++)
        printf("Line %d\n", i);
    printf("Done!\n");

    scheduling_exit();
}

static int vga_terminal_keyboard_input_handle_index;
void setup_scheduling(void)
{
    scheduling_init();

    syscalls_register(SYSCALL_EXIT, (void *)scheduling_exit);
    syscalls_register(SYSCALL_SLEEP, (void *)scheduling_sleep);
    syscalls_register(SYSCALL_YIELD, (void *)scheduling_yield);

    kthread_register((kthread_entry_t)kthread_idle, "idle-thread");
    kthread_register((kthread_entry_t)kthread_task_cleaner, "task-cleaner");
    kthread_register((kthread_entry_t)kthread_aids, "kthread_aids");

    kthread_register((kthread_entry_t)kthread_graphics, "vesa-graphics");
}
void finish_scheduling(void)
{
    kthread_start("idle-thread", 0, NULL);
    kthread_start("task-cleaner", 0, NULL);
    kthread_start("kthread_aids", 0, NULL);

    if (!kernel_context->video_state.is_text_mode)
        kthread_start("vesa-graphics", 0, NULL);

    pit_add_handle((pit_handle_t)scheduling_schedule);
}

// ------------------------------------------------------------
// Setup: Input

static void vga_terminal_keyboard_input_handle(keyboard_key_t keyboard_key, const keyboard_map_t keyboard_map)
{
    if (keyboard_key.value != '\0' && keyboard_key.is_pressed && !keyboard_key.previously_pressed)
    {
        if (isletter(keyboard_key.value) || isdigit(keyboard_key.value) || keyboard_key.value == '\n' || keyboard_key.value == '\b')
            terminal_add_key(keyboard_key.value);

        vga_terminal_write(&keyboard_key.value, 1);
    }
}
void enable_keyboard_input(void)
{
    vga_terminal_show_cursor(true);
    vga_terminal_keyboard_input_handle_index = keyboard_add_input_handle((keyboard_input_handle_t)vga_terminal_keyboard_input_handle);
}

// ------------------------------------------------------------
// Setup: VESA
#include "drivers/vesa/vesa.h"

int setup_vesa(void)
{
    const int min_width = 640;
    const int min_height = 400;
    if (vesa_init())
        return 1;

    vga_terminal_clear();

    vesa_mode_t *vesa_modes = vesa_get_all_modes();

    vga_terminal_write_string("VESA Options:\n");
    for (int i = 0; i < VESA_MODE_SIZE; i++)
    {
        if (vesa_modes[i].number == 0 || vesa_modes[i].info.bpp != 32)
            continue;

        if (vesa_modes[i].info.width < min_width || vesa_modes[i].info.height < min_height)
            continue;

        vga_terminal_write_string("Mode: %d %dx%d\n", vesa_modes[i].number, vesa_modes[i].info.width, vesa_modes[i].info.height);
    }

    char buffer[16];
    while (true)
    {
        vga_terminal_write_string("Choose a mode: ");

        while (!terminal_get_input(buffer, 16))
        {
            __asm__ volatile("hlt");
        }

        int mode = atoi(buffer);

        bool invalid_pick = false;
        for (int i = 0; i < VESA_MODE_SIZE; i++)
        {
            if (vesa_modes[i].number == mode)
            {
                if (vesa_modes[i].info.width < min_width || vesa_modes[i].info.height < min_height)
                {
                    invalid_pick = true;
                    break;
                }
            }
        }

        if (invalid_pick || vesa_set_mode(mode))
            continue;

        break;
    }

    keyboard_remove_input_handle(vga_terminal_keyboard_input_handle_index);
    vga_terminal_destroy();

    return 0;
}
