/// ------------
/// @file util.h

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

// kernel-main.c
extern int system_interrupt_disable_counter;

#define DISABLE_INTERRUPTS()            \
    system_interrupt_disable_counter++; \
    asm("cli");

#define ENABLE_INTERRUPTS()                    \
    system_interrupt_disable_counter--;        \
    if (system_interrupt_disable_counter == 0) \
    {                                          \
        asm("sti");                            \
    }

#define INTERRUPT_SAFE_BLOCK(code_block) \
    DISABLE_INTERRUPTS();                \
    do                                   \
    {                                    \
        code_block                       \
    } while (0);                          \
        ENABLE_INTERRUPTS()
    
#define PANIC() \
    asm("cli"); \
    while (1)

// kernel-main.c
void* kernel_malloc(uint32_t size);
void  kernel_free(void* ptr);
void* kernel_amalloc(uint32_t size);
void* kernel_calloc(uint32_t num, uint32_t size);
void* kernel_realloc(void* ptr, uint32_t new_size);

#include <kernel/data-structures/syscalls.h>

#endif