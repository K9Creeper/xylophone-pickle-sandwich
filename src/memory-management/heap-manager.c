/// --------------
/// heap-manager.c
/// @brief This file contains the definition of the heap_manager struct methods.

#include <memory-management/heap-manager.h>

#include <memory-management/paging-manager.h>

heap_manager_t *current_system_heap_manager = NULL;

static bool is_less_than(const type_t a, const type_t b)
{
    return (((heap_header_t *)(a))->size < ((heap_header_t *)(b))->size);
}

static int find_smallest_hole(heap_manager_t *heap, uint32_t size, bool should_align)
{
    uint32_t i = 0;

    while (i < ordered_array_size(&heap->heap_array))
    {
        heap_header_t *header = (heap_header_t *)ordered_array_get(&heap->heap_array, i);

        if (should_align)
        {
            uint32_t location = (uint32_t)header;
            uint32_t hole_start = location + sizeof(heap_header_t);
            int offset = 0;

            if ((hole_start & (PAGE_SIZE - 1)) != 0)
            {
                offset = PAGE_SIZE - (hole_start & (PAGE_SIZE - 1));
            }

            int hole_size = (int)header->size - offset;

            if (hole_size >= (int)size)
            {
                break;
            }
        }

        else if (header->size >= size)
        {
            break;
        }

        i++;
    }

    if (i == ordered_array_size(&heap->heap_array))
        return -1;

    return i;
}

static void expand(heap_manager_t *heap, uint32_t new_length)
{
    if (new_length == 0)
        return;

    if ((new_length & (PAGE_SIZE - 1)) != 0)
    {
        new_length &= ~(PAGE_SIZE - 1);
        new_length += PAGE_SIZE;
    }

    if (heap->use_identity_mapping)
    {
        paging_manager_identity_allocate_range(heap->paging_manager, heap->end_address, heap->start_address + new_length, heap->is_supervisor, !heap->is_readonly);
    }
    else
    {
        paging_manager_allocate_range(heap->paging_manager, heap->end_address, heap->start_address + new_length, heap->is_supervisor, !heap->is_readonly);
    }

    heap->end_address = heap->start_address + new_length;
}

static uint32_t contract(heap_manager_t *heap, uint32_t new_size)
{
    if ((new_size & (PAGE_SIZE - 1)) != 0)
    {
        new_size &= ~(PAGE_SIZE - 1);
        new_size += PAGE_SIZE;
    }

    if (new_size < HEAP_MIN_SIZE)
    {
        new_size = HEAP_MIN_SIZE;
    }

    uint32_t old_size = heap->end_address - heap->start_address;

    for (uint32_t i = old_size - PAGE_SIZE; new_size < i; i -= PAGE_SIZE)
    {
        paging_manager_free_single(heap->paging_manager, i, true);
    }

    heap->end_address = heap->start_address + new_size;

    return new_size;
}

static void *allocate(heap_manager_t *heap, uint32_t size, bool should_align, uint8_t iterration)
{
    uint32_t new_size = size + sizeof(heap_header_t) + sizeof(heap_footer_t);
    int i = find_smallest_hole(heap, new_size, should_align);
    if (i == -1)
    {
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t old_end_address = heap->end_address;
        uint32_t new_length = old_length + new_size;

        if (heap->start_address + new_length > heap->max_end_address || iterration > 0)
        {
            return NULL;
        }

        expand(heap, old_length + new_size);

        new_length = heap->end_address - heap->start_address;

        i = 0;
        bool found = false;
        uint32_t idx = 0;
        uint32_t value = 0;

        while (i < ordered_array_size(&heap->heap_array))
        {
            uint32_t tmp = (uint32_t)(ordered_array_get(&heap->heap_array, i));

            if (tmp > value)
            {
                value = tmp;
                idx = i;
                found = true;
            }
            i++;
        }

        if (!found)
        {
            heap_header_t *header = (heap_header_t *)(old_end_address);
            header->size = new_length - old_length;
            header->magic = HEAP_MAGIC;
            header->is_hole = true;

            heap_footer_t *footer = (heap_footer_t *)(old_end_address + header->size - sizeof(heap_footer_t));
            footer->header = header;
            footer->magic = HEAP_MAGIC;

            ordered_array_insert(&heap->heap_array, (type_t)header);
        }
        else
        {
            heap_header_t *header = (heap_header_t *)(ordered_array_get(&heap->heap_array, idx));
            header->size += new_length - old_length;

            uint32_t footer_addr = (uint32_t)(header) + header->size - sizeof(heap_footer_t);

            heap_footer_t *footer = (heap_footer_t *)(footer_addr);
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }

        return allocate(heap, size, should_align, ++iterration);
    }

    heap_header_t *orig_hole_header = (heap_header_t *)(ordered_array_get(&heap->heap_array, i));
    uint32_t orig_hole_pos = (uint32_t)(orig_hole_header);
    uint32_t orig_hole_size = orig_hole_header->size;

    uint32_t leftover = orig_hole_size - new_size;
    if (leftover < sizeof(heap_header_t) + sizeof(heap_footer_t))
    {
        size += leftover;
        new_size = orig_hole_size;
    }

    if (should_align && ((orig_hole_pos & 0xFFFFF000) != 0))
    {
        uint32_t size2, footer_pos;

        size2 = PAGE_SIZE - (orig_hole_pos & 0xFFF) - sizeof(heap_header_t);

        heap_header_t *hole_header = (heap_header_t *)(orig_hole_pos);
        hole_header->size = size2;
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = true;

        footer_pos = orig_hole_pos + size2 - sizeof(heap_footer_t);
        heap_footer_t *hole_footer = (heap_footer_t *)footer_pos;
        hole_footer->header = hole_header;
        hole_footer->magic = HEAP_MAGIC;

        orig_hole_pos += size2;
        orig_hole_size -= size2;
    }
    else
        ordered_array_remove(&heap->heap_array, i);

    heap_header_t *block_header = (heap_header_t *)orig_hole_pos;
    block_header->size = new_size;
    block_header->magic = HEAP_MAGIC;
    block_header->is_hole = false;

    heap_footer_t *block_footer = (heap_footer_t *)((uint32_t)orig_hole_pos + sizeof(heap_header_t) + size);
    block_footer->header = block_header;
    block_footer->magic = HEAP_MAGIC;

    if (orig_hole_size - new_size > 0)
    {
        heap_header_t *hole_header = (heap_header_t *)(orig_hole_pos + sizeof(heap_header_t) + size + sizeof(heap_footer_t));
        hole_header->size = orig_hole_size - new_size;
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = true;

        heap_footer_t *hole_footer = (heap_footer_t *)((uint32_t)hole_header + orig_hole_size - new_size - sizeof(heap_footer_t));

        if ((uint32_t)hole_footer < heap->end_address)
        {
            hole_footer->header = hole_header;
            hole_footer->magic = HEAP_MAGIC;
        }

        ordered_array_insert(&heap->heap_array, (type_t)hole_header);
    }

    return (void *)((uint32_t)block_header + sizeof(heap_header_t));
}

static void free_(heap_manager_t *heap, uint32_t address)
{
    if (!(void *)address)
        return;

    uint32_t i;

    heap_header_t *header = (heap_header_t *)(address - sizeof(heap_header_t));
    heap_footer_t *footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));

    header->is_hole = true;

    bool should_add = true;

    heap_footer_t *test_footer = (heap_footer_t *)((uint32_t)header - sizeof(heap_footer_t));

    if (test_footer->magic == HEAP_MAGIC && test_footer->header->is_hole)
    {
        uint32_t cache_size = header->size;

        header = test_footer->header;
        footer->header = header;

        header->size += cache_size;

        should_add = false;
    }

    heap_header_t *test_header = (heap_header_t *)(footer + sizeof(heap_footer_t));

    if (test_header->magic == HEAP_MAGIC && test_header->is_hole)
    {
        header->size += test_header->size;

        test_footer = (heap_footer_t *)((uint32_t)test_header + test_header->size - sizeof(heap_footer_t));
        footer = test_footer;

        i = 0;
        while ((i < ordered_array_size(&heap->heap_array)) &&
               (((void *)ordered_array_get(&heap->heap_array, i)) != (void *)test_header))
        {
            i++;
        }

        ordered_array_remove(&heap->heap_array, i);
    }

    if ((uint32_t)(footer) + sizeof(heap_footer_t) == heap->end_address)
    {
        uint32_t old_length = heap->end_address - heap->start_address;
        uint32_t new_length = contract(heap, (uint32_t)header - heap->start_address);

        if (header->size - (old_length - new_length) > 0)
        {
            header->size -= (old_length - new_length);

            footer = (heap_footer_t *)((uint32_t)header + header->size - sizeof(heap_footer_t));

            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }
        else
        {
            while ((i < ordered_array_size(&heap->heap_array)) && (((void *)ordered_array_get(&heap->heap_array, i)) != (void *)test_header))
            {
                i++;
            }

            if (i < ordered_array_size(&heap->heap_array))
            {
                ordered_array_remove(&heap->heap_array, i);
            }
        }
    }

    if (should_add)
    {
        ordered_array_insert(&heap->heap_array, (type_t)header);
    }
}

void heap_manager_pre_init(heap_manager_t *heap, uint32_t end)
{
    if (!heap)
        return;

    if (heap->status > HEAP_UNINITIALIZED)
        return;

    heap->placement_address = end;
    heap->status = HEAP_PRE_INITIALIZED;
}

void heap_manager_init(heap_manager_t *heap, uint32_t start_address, uint32_t end_address, uint32_t max_end_address, bool is_supervisor, bool is_readonly, bool use_identity_mapping, paging_manager_t *paging_manager)
{
    if (!heap || !paging_manager || !paging_manager->is_initialized)
        return;

    if (heap->status != HEAP_PRE_INITIALIZED)
        return;

    heap->paging_manager = paging_manager;

    heap->is_supervisor = is_supervisor;
    heap->is_readonly = is_readonly;
    heap->use_identity_mapping = use_identity_mapping;
    
    if (use_identity_mapping)
    {
        paging_manager_identity_allocate_range(paging_manager, start_address, end_address, is_supervisor, !is_readonly);
    }
    else
    {
        paging_manager_allocate_range(paging_manager, start_address, end_address, is_supervisor, !is_readonly);
    }

    ordered_array_init(&heap->heap_array, HEAP_INDEX_SIZE, is_less_than);
    ordered_array_place(&heap->heap_array, (void *)start_address);

    start_address += sizeof(void *) * HEAP_INDEX_SIZE;

    if ((start_address & 0xFFFFF000) != 0)
    {
        start_address &= 0xFFFFF000;
        start_address += PAGE_SIZE;
    }

    heap->start_address = start_address;
    heap->end_address = end_address;
    heap->max_end_address = max_end_address;

    heap_header_t *hole = (heap_header_t *)start_address;
    hole->size = end_address - start_address;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = true;

    ordered_array_insert(&heap->heap_array, hole);

    if (!current_system_heap_manager)
        current_system_heap_manager = heap;

    heap->status = HEAP_INITIALIZED;
}

void *heap_manager_malloc(heap_manager_t *heap, uint32_t size, bool should_align, uint32_t *physical_address)
{
    if (heap->status == HEAP_INITIALIZED)
    {
        uint32_t address = (uint32_t)allocate(heap, size, should_align, 0);

        if (physical_address)
        {
            *physical_address = paging_manager_get_physical_address(heap->paging_manager, address);
        }

        return (void *)address;
    }
    else if (heap->status == HEAP_PRE_INITIALIZED)
    {
        if (should_align && (heap->placement_address & 0x00000FFF))
        {
            heap->placement_address &= 0xFFFFF000;
            heap->placement_address += PAGE_SIZE;
        }

        uint32_t base = heap->placement_address;

        if (physical_address)
            *physical_address = base;

        heap->placement_address += size;

        return (void *)base;
    }

    return NULL;
}

void heap_manager_free(heap_manager_t *heap, uint32_t address)
{
    if (heap->status == HEAP_INITIALIZED)
    {
        free_(heap, address);
    }
}