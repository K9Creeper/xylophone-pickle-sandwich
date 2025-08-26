/// -------------------------------
/// @file physical-memory-manager.c

#include <kernel/memory-management/physical-memory-manager.h>
#include <kernel/data-structures/memory-management/paging-structures.h>

#include <kernel/memory-management/heap-manager.h>

void physical_memory_manager_init(physical_memory_manager_t *physical_memory_manager, uint32_t memory_size, physical_memory_manager_failure_handle_t failure_handle, heap_manager_t* heap_manager)
{
    if (!physical_memory_manager || physical_memory_manager->is_initialized)
        return;

    physical_memory_manager->failed = 0;
    physical_memory_manager->failure = failure_handle;
    
    bitmap_init(&physical_memory_manager->bitmap, sizeof(uint32_t));

    const int bit_count = memory_size / PAGE_SIZE;
    uint32_t entry_count = bit_count / physical_memory_manager->bitmap.bits_per_entry;
    const int size = entry_count * physical_memory_manager->bitmap.sizeof_type_t;

    void *array = heap_manager_malloc(heap_manager, size, 1, NULL);
    if (array != NULL)
    {
        bitmap_place(&physical_memory_manager->bitmap, array, size);
        bitmap_clear(&physical_memory_manager->bitmap);

        physical_memory_manager->is_initialized = 1;
    }
}

uint32_t physical_memory_manager_allocate_block(physical_memory_manager_t *physical_memory_manager)
{
    if (!physical_memory_manager || !physical_memory_manager->is_initialized)
        return (uint32_t)(-1);

    uint32_t free_block;

    uint8_t found = physical_memory_manager_get_first_free_block(physical_memory_manager, &free_block);
    if (found)
    {
        uint32_t index = bitmap_index_from_bit(&physical_memory_manager->bitmap, free_block);
        uint32_t offset = bitmap_offset_from_bit(&physical_memory_manager->bitmap, free_block);

        physical_memory_manager->bitmap.array[index] |= (0x1 << offset);

        return free_block;
    }

    return (uint32_t)(-1);
}

void physical_memory_manager_free_block(physical_memory_manager_t *physical_memory_manager, uint32_t block_number)
{
    if (!physical_memory_manager || !physical_memory_manager->is_initialized)
        return;
    uint32_t index = bitmap_index_from_bit(&physical_memory_manager->bitmap, block_number);
    uint32_t offset = bitmap_offset_from_bit(&physical_memory_manager->bitmap, block_number);

    physical_memory_manager->bitmap.array[index] &= ~(0x1 << offset);
}

uint8_t physical_memory_manager_get_first_free_block(physical_memory_manager_t *physical_memory_manager, uint32_t *out)
{
    if (!physical_memory_manager || !physical_memory_manager->is_initialized)
        return 0;

    for (uint32_t i = 0; i < physical_memory_manager->bitmap.bit_count; i++)
    {
        if (!((physical_memory_manager->bitmap.array[bitmap_index_from_bit(&physical_memory_manager->bitmap, i)] >> (bitmap_offset_from_bit(&physical_memory_manager->bitmap, i))) & 0x1))
        {
            *out = i;
            return 1;
        }
    }
    return 0;
}