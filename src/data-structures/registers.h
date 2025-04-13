/// -----------
/// registers.h
/// @brief This file declares the register structures.

#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

struct Registers
{
    uint32_t ds; // 0

    uint32_t edi, // 4
        esi,      // 8
        ebp,      // 12
        esp,      // 16
        ebx,      // 20
        edx,      // 24
        ecx,      // 28
        eax;      // 32

    uint32_t int_no, // 36
        err_code;    // 40

    uint32_t eip, // 44
        cs,       // 48
        eflags,   // 52
        useresp,  // 56
        ss;       // 60
};

struct Registers16
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
};

#endif