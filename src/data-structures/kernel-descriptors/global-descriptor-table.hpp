/// ---------------------------
/// global-descriptor-table.hpp
/// @brief This file defines the structures to the Global Descriptor Table.

#pragma once

#include <stdint.h>

struct Global_Descriptor_Table_Entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct Global_Descriptor_Table_Pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));