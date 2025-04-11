/// ---------------------------
/// interrupt-descriptor-table.hpp
/// @brief This file declares the structures to the Interrupt Descriptor Table.

#pragma once

#include <stdint.h>

struct Interrupt_Descriptor_Table_Entry
{
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct Interrupt_Descriptor_Table_Pointer
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));