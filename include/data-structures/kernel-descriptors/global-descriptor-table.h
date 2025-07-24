/// -------------------------
/// global-descriptor-table.h
/// @brief This file defines the structures to the Global Descriptor Table.

#ifndef GLOBAL_DESCRIPTOR_TABLE_H
#define GLOBAL_DESCRIPTOR_TABLE_H

#include <stdint.h>

typedef struct global_descriptor_table_entry_s
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) global_descriptor_table_entry_t;

typedef struct global_descriptor_table_ptr_s
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) global_descriptor_table_ptr_t;

#endif