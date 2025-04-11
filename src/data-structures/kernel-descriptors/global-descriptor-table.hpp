/// ---------------------------
/// global-descriptor-table.hpp
/// @brief This file declares the structures to the Global Descriptor Table.

#pragma once

#include <stdint.h>

struct Global_Descriptor_Table_Entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct Global_Descriptor_Table_Pointer
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));