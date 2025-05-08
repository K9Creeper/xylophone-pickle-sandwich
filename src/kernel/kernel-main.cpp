/// ---------------
/// kernel-main.cpp
/// @breif This is the C/C++ entry of the kernel.

#define cli asm volatile("cli")
#define sti asm volatile("sti")

// COM1
extern "C" void printf(const char *f, ...);

// Terminal Print (wrapper)
static void terminalf(const char*f, ...);

#include <stdint.h>
#include "../../multiboot2.h"

// Wrapper function
static void Setup_Serial_Com1();

// Wrapper function
static void Setup_Terminal_Display();

// Wrapper function
static void Setup_Descriptors();

extern "C" void kernel_main(uint32_t addr, uint32_t magic)
{
    cli;
    Setup_Terminal_Display();

    Setup_Serial_Com1();

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) goto halt;

    terminalf("Valid Multiboot Magic.\n");

    Setup_Descriptors();
    
    terminalf("Descriptors Initialized.\n");

    halt:
    for (;;)
    {
        asm volatile("hlt");
    }
}

#include <stdarg.h>

#include "misc/serial-com/serial-com.hpp"
#include "misc/vga-terminal/vga-terminal.hpp"

#include "descriptors/global-descriptor-table.hpp"
#include "descriptors/interrupt-descriptor-table.hpp"

static void terminalf(const char* f, ...) {
    va_list args;
    va_start(args, f);
    Kernel::Misc::VGA_Terminal_writestring(f, args);
    va_end(args);
}

static void Setup_Serial_Com1(){
    Kernel::Misc::Serial_Com_Init();
}

static void Setup_Terminal_Display(){
    Kernel::Misc::VGA_Terminal_Init();
}

static void Setup_Descriptors(){
    Kernel::Descriptors::Global_Descriptor_Table_Init();
    Kernel::Descriptors::Global_Descriptor_Table_Install();

    Kernel::Descriptors::Interrupt_Descriptor_Table_Init();
    Kernel::Descriptors::Interrupt_Descriptor_Table_Install();
}