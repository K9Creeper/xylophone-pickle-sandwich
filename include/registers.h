/// -----------
/// registers.h
/// @brief This file declares the register structures.

#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct registers_s {
    uint32_t ds; // 0
    //        4    8    12  16  20    24   28   32
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    //         36         40
    uint32_t interrupt, error;

    //        44  48   52        56            60
    uint32_t eip, cs, eflags, usermode_esp, usermode_ss;
} registers_t;

typedef struct registers16_s
{
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;

    uint16_t ds;
    uint16_t es;
    uint16_t fs;
    uint16_t gs;
    uint16_t ss;
    uint16_t eflags;
} registers16_t;

#endif