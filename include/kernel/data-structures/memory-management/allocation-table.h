/// ------------------------
/// @file allocation-table.h

#ifndef ALLOCATION_TABLE_H
#define ALLOCATION_TABLE_H

#include <stdint.h>

#define ALLOCATION_MAX_COUNT 512

typedef struct allocation_table_entry_s {
    void* address;
    uint32_t size;
}allocation_table_entry_t;

typedef struct allocation_table_s {
    uint32_t size;
    allocation_table_entry_t data[ALLOCATION_MAX_COUNT];
}allocation_table_t;

#endif