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

// Setup function declarations
static void setup_misc(void);
static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);
static void setup_gdt(void);
static void setup_idt(void);
static void setup_fault_handlers(void);
static int setup_vesa(void);
static void setup_disk_manager(void);
static void setup_drivers(void);
static void setup_filesystem(void);
static void setup_scheduling(void);
static void finish_scheduling(void);
static void enable_keyboard_input(void);

void kernel_main(uint32_t addr, uint32_t magic)
{
    DISABLE_INTERRUPTS();
    vga_terminal_write_string("Kernel: Interrupts disabled\n");

    setup_misc();
    vga_terminal_write_string("Kernel: Misc setup done\n");

    setup_gdt();
    vga_terminal_write_string("Kernel: GDT setup done\n");

    setup_idt();
    vga_terminal_write_string("Kernel: IDT setup done\n");

    setup_early_heap();
    vga_terminal_write_string("Kernel: Early heap setup done\n");

    setup_paging();
    vga_terminal_write_string("Kernel: Paging setup done\n");

    setup_heap();
    vga_terminal_write_string("Kernel: Heap setup done\n");

    setup_disk_manager();
    vga_terminal_write_string("Kernel: Disk setup done\n");

    setup_drivers();
    vga_terminal_write_string("Kernel: Drivers setup done\n");

    setup_filesystem();
    vga_terminal_write_string("Kernel: Filesystem setup done\n");

    setup_fault_handlers();
    vga_terminal_write_string("Kernel: Fault handlers setup done\n");

    ENABLE_INTERRUPTS();
    vga_terminal_write_string("Kernel: Interrupts enabled\n");

    enable_keyboard_input();
    vga_terminal_write_string("Kernel: Keyboard input enabled\n");

    char buffer[4];
    while (true)
    {
        vga_terminal_write_string("Enter a graphical interface (y/n): ");

        while (!terminal_get_input(buffer, sizeof(buffer)))
        {
            __asm__ volatile("hlt");
        }

        if (strcmp(buffer, "y") == 0)
        {
            vga_terminal_write_string("User selected graphical interface.\n");

            if (setup_vesa())
            {
                vga_terminal_write_string("Graphical interface is unavailable\n");
                continue;
            }
            vga_terminal_write_string("Graphical interface initialized.\n");
            break;
        }
        else if (strcmp(buffer, "n") == 0)
        {
            vga_terminal_write_string("User selected text mode.\n");
            break;
        }
        else
        {
            vga_terminal_write_string("Invalid input. Please enter 'y' or 'n'.\n");
        }
    }

    DISABLE_INTERRUPTS();
    vga_terminal_write_string("Kernel: Interrupts disabled before scheduling setup\n");

    setup_scheduling();
    vga_terminal_write_string("Kernel: Scheduling setup done\n");

    finish_scheduling();
    vga_terminal_write_string("Kernel: Scheduling started\n");

    ENABLE_INTERRUPTS();
    vga_terminal_write_string("Kernel: Interrupts enabled after scheduling\n");

halt:
    vga_terminal_write_string("Kernel halted.\n");
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
    vga_terminal_init(DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS, VGA_TERMINAL_COLOR_MAGENTA, VGA_TERMINAL_COLOR_DARK_GREY);
    vga_terminal_show_cursor(false);
}

// ------------------------------------------------------------
// Setup: Descriptors
#include "descriptors/global-descriptor-table.h"
#include "bios32/bios32.h"
#include "descriptors/task-state-segment.h"
#include "descriptors/interrupt-descriptor-table.h"

// ../interrupts/interrupt-request.c
extern void kernel_interrupt_request_remap(void);

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
    kernel_interrupt_request_remap();
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

    uint32_t size_mb = kernel_context->physical_memory_size / (1024 * 1024);
    if (size_mb > 0)
    {
        vga_terminal_write_string("Kernel: Physical Memory setup for %d MB\n", size_mb);
    }
    else
    {
        uint32_t size_kb = kernel_context->physical_memory_size / 1024;
        vga_terminal_write_string("Kernel: Physical Memory setup for %d KB\n", size_kb);
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
// Setup: Disk
#include "disk-manager/disk-manager.h"
void setup_disk_manager(void)
{
    kdisk_manager_init();
}

// ------------------------------------------------------------
// Setup: Drivers
#include "drivers/pit/pit.h"
#include "drivers/pci/pci.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/mouse/mouse.h"
#include "drivers/syscalls/syscalls.h"
void test(registers_t *d, uint32_t t)
{
    if (t % pit_get_hz() == 0)
        vga_terminal_write_string("Tick %d\n", t);
}

void setup_drivers(void)
{
    syscalls_init();
    keyboard_init();
    mouse_init();
    pci_init();
    // Should maybe go below 1000hz?
    pit_init(1000);

    // pit_add_handle((pit_handle_t)test);
}

// ------------------------------------------------------------
// Setup: Filesystem
#include "filesystem/filesystem.h"
void setup_filesystem(void)
{
    uint8_t count = 0;
    disk_device_t *devices = disk_manager_get_devices(&kernel_context->disk_manager, &count);
    vga_terminal_write_string("Kernel: Disks %d\n", count);
    for (uint8_t i = 0; i < count; i++)
    {
        ide_device_t *dev = devices[i].dev;
        vga_terminal_write_string(" Disk %d - %s\n", i, dev->model);
    }

    kernel_filesystem_init();
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
        __asm__ volatile("hlt");
    }
}

void kthread_aids(void)
{
    for (int i = 0; i < 10; i++);
    
    printf("Done!\n");

    scheduling_exit();
}

// scheduling.c
extern void *syscall_malloc(int size);
extern void syscall_free(void *address);

void setup_scheduling(void)
{
    scheduling_init();

    syscalls_register(SYSCALL_EXIT, (void *)scheduling_exit);
    syscalls_register(SYSCALL_SLEEP, (void *)scheduling_sleep);
    syscalls_register(SYSCALL_YIELD, (void *)scheduling_yield);

    syscalls_register(SYSCALL_MALLOC, (void *)syscall_malloc);
    syscalls_register(SYSCALL_FREE, (void *)syscall_free);

    syscalls_register(SYSCALL_GET_SYSTEM_TICK_COUNT, (void *)pit_get_tick);

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
    {
        kthread_start("vesa-graphics", 0, NULL);
    }

    pit_add_handle((pit_handle_t)scheduling_schedule);
}

// ------------------------------------------------------------
// Setup: Input

static void vga_terminal_keyboard_input_handle(keyboard_key_t keyboard_key, const keyboard_map_t keyboard_map)
{
    if (keyboard_key.value != '\0' && keyboard_key.is_pressed)
    {
        if (isletter(keyboard_key.value) || isdigit(keyboard_key.value) || keyboard_key.value == '\n' || keyboard_key.value == '\b')
            terminal_add_key(keyboard_key.value);

        vga_terminal_write(&keyboard_key.value, 1);
    }
}
void enable_keyboard_input(void)
{
    vga_terminal_show_cursor(true);
    keyboard_add_input_handle((keyboard_input_handle_t)vga_terminal_keyboard_input_handle);
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

    // Gather valid modes into a temporary list for indexed selection
    typedef struct
    {
        int index;
        int mode_number;
        int width;
        int height;
    } selectable_mode_t;

    selectable_mode_t valid_modes[VESA_MODE_SIZE];
    int valid_count = 0;

    vga_terminal_write_string("Available VESA modes (select by number):\n");
    for (int i = 0; i < VESA_MODE_SIZE; i++)
    {
        if (vesa_modes[i].number == 0 || vesa_modes[i].info.bpp != 32)
            continue;

        if (vesa_modes[i].info.width < min_width || vesa_modes[i].info.height < min_height)
            continue;

        valid_modes[valid_count].index = valid_count + 1;
        valid_modes[valid_count].mode_number = vesa_modes[i].number;
        valid_modes[valid_count].width = vesa_modes[i].info.width;
        valid_modes[valid_count].height = vesa_modes[i].info.height;

        vga_terminal_write_string("  %d) %dx%d\n",
                                  valid_modes[valid_count].index,
                                  valid_modes[valid_count].width,
                                  valid_modes[valid_count].height);

        valid_count++;
    }

    if (valid_count == 0)
    {
        vga_terminal_write_string("No valid VESA modes found.\n");
        return 1;
    }

    char buffer[16];
    while (true)
    {
        vga_terminal_write_string("Choose a mode number (or 'q' to quit): ");

        while (!terminal_get_input(buffer, sizeof(buffer)));

        if (buffer[0] == 'q' || buffer[0] == 'Q')
        {
            vga_terminal_write_string("Cancelled mode selection.\n");
            return 1; // cancel
        }

        int choice = atoi(buffer);

        if (choice < 1 || choice > valid_count)
        {
            vga_terminal_write_string("Invalid selection. Please enter a valid number.\n");
            continue;
        }

        int selected_mode = valid_modes[choice - 1].mode_number;
        vga_terminal_write_string("Selected mode: %d - %dx%d\n",
                                  selected_mode,
                                  valid_modes[choice - 1].width,
                                  valid_modes[choice - 1].height);

        if (vesa_set_mode(selected_mode))
        {
            vga_terminal_write_string("Failed to set the selected mode. Try another.\n");
            continue;
        }

        break;
    }

    keyboard_remove_input_handle();
    vga_terminal_destroy();

    return 0;
}