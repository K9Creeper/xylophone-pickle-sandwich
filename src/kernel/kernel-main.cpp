/// ---------------
/// kernel-main.cpp
/// @breif This is the C/C++ entry of the kernel.

#define cli asm volatile("cli")
#define sti asm volatile("sti")

// COM1
extern "C" void printf(const char *f, ...);

#include <stdint.h>
#include "../../multiboot2.h"

// Wrapper functions
static void Setup_Serial_Com1();
static void Setup_Terminal_Display();
static void Setup_Descriptors();

static void Setup_Early_Heap();
static void Setup_Paging();
static void Setup_Heap();

#include "misc/vga-terminal/vga-terminal.hpp"

extern "C" void kernel_main(uint32_t addr, uint32_t magic)
{
    cli;
    Setup_Serial_Com1();

    Setup_Terminal_Display();

    Kernel::Misc::VGA_Terminal_writestring("Setting-up Descriptors\n");
    Setup_Descriptors();

    Kernel::Misc::VGA_Terminal_writestring("Setting-up Early Kernel Heap\n");
    Setup_Early_Heap();

    Kernel::Misc::VGA_Terminal_writestring("Setting-up Kernel Paging\n");
    Setup_Paging();

    Kernel::Misc::VGA_Terminal_writestring("Finishing Setting-up Kernel Heap\n");
    Setup_Heap();

    sti;

halt:
    for (;;)
    {
        asm volatile("hlt");
    }
}

#include "misc/serial-com/serial-com.hpp"

#include "descriptors/global-descriptor-table.hpp"
#include "descriptors/interrupt-descriptor-table.hpp"

static void Setup_Serial_Com1()
{
    Kernel::Misc::Serial_Com_Init();
}

static void Setup_Terminal_Display()
{
    Kernel::Misc::VGA_Terminal_Init();
}

static void Setup_Descriptors()
{
    Kernel::Descriptors::Global_Descriptor_Table_Init();
    Kernel::Descriptors::Global_Descriptor_Table_Install();

    Kernel::Descriptors::Interrupt_Descriptor_Table_Init();
    Kernel::Descriptors::Interrupt_Descriptor_Table_Install();
}

#include "memory-management/pmm.hpp"
#include "memory-management/heap.hpp"
#include "memory-management/paging.hpp"

static void Setup_Early_Heap()
{
    Kernel::Memory_Management::Heap_Pre_Init();
}

static void Setup_Paging()
{
    Kernel::Memory_Management::PMM_Init(0x1000000);

    Kernel::Memory_Management::Paging_Init();
}

static void Setup_Heap()
{
    Kernel::Memory_Management::Heap_Init();
}