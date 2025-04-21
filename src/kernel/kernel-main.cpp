/// ---------------
/// kernel-main.cpp
/// @breif This is the C/C++ entry of the kernel.

#define cli asm volatile("cli")
#define sti asm volatile("sti")

// COM1
extern "C" void printf(const char* f, ...);

#include <stdint.h>
#include "../../multiboot2.h"

#include "misc/serial-com/serial-com.hpp"

#include "misc/vga-terminal/vga-terminal.hpp"

extern "C" void kernel_main(uint32_t addr, uint32_t magic)
{
    cli;

    Kernel::Misc::Serial_Com_Init();

    Kernel::Misc::VGA_Terminal_Init();

    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        goto halt;

    halt:
        for(;;){
            asm volatile("hlt");
        }
}