/// ----------------------------
/// interrupt-descriptor-table.h
/// @brief This file defines the structures to the Interrupt Descriptor Table.

#ifndef INTERRUPT_DESCRIPTOR_TABLE_H
#define INTERRUPT_DESCRIPTOR_TABLE_H

#include <stdint.h>

typedef struct interrupt_descriptor_table_entry_s
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) interrupt_descriptor_table_entry_t;

typedef struct interrupt_descriptor_table_ptr_s
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) interrupt_descriptor_table_ptr_t;

#endif