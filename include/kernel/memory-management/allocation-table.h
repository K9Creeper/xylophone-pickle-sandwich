/// ------------------------
/// @file allocation-table.h

#ifndef _ALLOCATION_TABLE_H
#define _ALLOCATION_TABLE_H

#include <kernel/data-structures/memory-management/allocation-table.h>

extern void allocation_table_init(allocation_table_t* table);

extern void allocation_table_add(allocation_table_t *table, uint32_t address, uint32_t size);
extern void allocation_table_remove(allocation_table_t* table, uint32_t address);

#endif