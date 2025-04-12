/// ---------------------------
/// interrupt-descriptor-table.hpp
/// @brief This file defines the structures to the Interrupt Descriptor Table.

#pragma once

#include <stdint.h>

struct Interrupt_Descriptor_Table_Entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

struct Interrupt_Descriptor_Table_Pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));