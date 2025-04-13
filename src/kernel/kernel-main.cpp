/// ---------------
/// kernel-main.cpp
/// @breif This is the C/C++ entry of the kernel.

#define cli asm volatile("cli")
#define sti asm volatile("sti")

// COM1
extern "C" void printf(const char* f, ...);

#include <stdint.h>

#include "misc/serial.hpp"

extern "C" void kernel_main(uint32_t addr, uint32_t magic)
{
    cli;

    Kernel::Misc::COM1_Init();

    printf("Hello World\n");

    for(;;){
        asm volatile("hlt");
    }
}