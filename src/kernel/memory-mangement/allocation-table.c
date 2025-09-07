/// ------------------------
/// @file allocation-table.c

#include <kernel/memory-management/allocation-table.h>

#include <stddef.h>
#include <memory.h>

static allocation_table_entry_t *find_free_entry(allocation_table_t *table)
{
    for (int i = 0; i < ALLOCATION_MAX_COUNT; i++)
    {
        if (table->data[i].address == NULL && table->data[i].size == 0)
        {
            return &table->data[i];
        }
    }
    return NULL;
}

void allocation_table_init(allocation_table_t *table)
{
    table->size = 0;
    memset((uint8_t*)table->data, 0, sizeof(table->data));
}

void allocation_table_add(allocation_table_t *table, uint32_t address, uint32_t size)
{
    allocation_table_entry_t *entry = find_free_entry(table);
    if (entry == NULL)
        return;

    entry->address = (void*)address;
    entry->size = size;
    table->size++;
}

void allocation_table_remove(allocation_table_t *table, uint32_t address)
{
    for (int i = 0; i < ALLOCATION_MAX_COUNT; i++)
    {
        if ((uint32_t)table->data[i].address == address)
        {
            table->data[i].address = NULL;
            table->data[i].size = 0;
            table->size--;
            break;
        }
    }
}