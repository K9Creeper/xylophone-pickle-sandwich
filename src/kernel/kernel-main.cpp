/// ---------------
/// kernel-main.cpp
/// @breif This is the C/C++ entry of the kernel.

#include <stdint.h>

extern "C" void kernel_main(uint32_t addr, uint32_t magic)
{
    

    for(;;){
        asm volatile("hlt");
    }
}