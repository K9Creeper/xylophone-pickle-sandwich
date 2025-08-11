/// ------
/// util.h
/// @brief This file defines macros.

#ifndef UTIL_H
#define UTIL_H

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
        ENABLE_INTERRUPTS();

#define PANIC() \
    asm("cli"); \
    while (1)

#endif