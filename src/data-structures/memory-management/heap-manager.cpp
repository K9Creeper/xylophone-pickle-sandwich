/// ----------------
/// heap-manager.hpp
/// This file contains the definition of the Heap_Manager class methods.

#include "heap-manager.hpp"

#include "paging-manager.hpp"

uint32_t Heap_Manager::Get_Start_Address() const
{
    return start_address;
}

uint32_t Heap_Manager::Get_End_Address() const
{
    return end_address;
}

uint32_t Heap_Manager::Get_Max_End_Address() const
{
    return max_end_address;
}

void Heap_Manager::Pre_Initialize(uint32_t end)
{
    if (bPreInitialized)
        return;

    placement_address = end;

    bPreInitialized = true;
}

void Heap_Manager::Initialize(uint32_t start_address, uint32_t end_address, uint32_t max_end_address, bool is_supervisor, bool is_readonly, Paging_Manager *paging_manager)
{
    if (bInitialized || !bPreInitialized || !paging_manager)
        return;

    this->paging_manager = paging_manager;

    this->is_supervisor = is_supervisor;
    this->is_readonly = is_readonly;

    this->paging_manager->Allocate(start_address, end_address, is_supervisor, !is_readonly);

    heap_array.Place(reinterpret_cast<void *>(start_address), HEAP_INDEX_SIZE);

    start_address += sizeof(void *) * HEAP_INDEX_SIZE;

    if ((start_address & 0xFFFFF000) != 0)
    {
        start_address &= 0xFFFFF000;
        start_address += 0x1000;
    }

    this->start_address = start_address;
    this->end_address = end_address;
    this->max_end_address = max_end_address;

    Heap_Header *hole = reinterpret_cast<Heap_Header *>(start_address);
    hole->size = end_address - start_address;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = true;

    heap_array.Insert(reinterpret_cast<void *>(hole));

    bInitialized = true;
}

int Heap_Manager::Find_Smallest_Hole(uint32_t size, bool should_align)
{
    uint32_t i = 0;
    for (; i < heap_array.Get_Size(); i++)
    {
        Heap_Header *header = reinterpret_cast<Heap_Header *>(heap_array.Get(i));
        if (should_align)
        {
            uint32_t location = reinterpret_cast<uint32_t>(header);
            uint32_t hole_start = location + sizeof(Heap_Header);
            uint32_t offset = 0;

            if ((location + sizeof(Heap_Header) & 0xFFFFF000) != 0)
            {
                offset = 0x1000 - (hole_start % 0x1000);
            }

            uint32_t hole_size = header->size - offset;

            if (hole_size >= size)
                break;
        }
        else if (header->size >= size)
            break;
    }

    if (i == heap_array.Get_Size())
        return -1;

    return i;
}

void Heap_Manager::Expand(uint32_t new_size)
{
    if (new_size <= 0)
        return;

    if ((new_size & 0xFFFFF000) != 0)
    {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    paging_manager->Allocate(end_address, start_address + new_size, is_supervisor, !is_readonly);

    end_address = start_address + new_size;
}

uint32_t Heap_Manager::Contract(uint32_t new_size)
{
    if (new_size & 0x1000)
    {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    if (new_size < HEAP_MIN_SIZE)
    {
        new_size = HEAP_MIN_SIZE;
    }

    uint32_t old_size = end_address - start_address;

    for (uint32_t i = old_size - 0x1000; new_size < i; i -= 0x1000)
    {
        paging_manager->Free(i, true);
    }

    end_address = start_address + new_size;

    return new_size;
}

void *Heap_Manager::Allocate(uint32_t size, bool should_align)
{
    uint32_t new_size = size + sizeof(Heap_Header) + sizeof(Heap_Footer);

    int i = Find_Smallest_Hole(new_size, should_align);

    if (i == -1)
    {
        uint32_t old_length = end_address - start_address;
        uint32_t old_end_address = end_address;

        Expand(old_length + new_size);

        uint32_t new_length = end_address - start_address;

        i = 0;
        bool found = false;
        uint32_t idx = 0;
        uint32_t value = 0;

        while (i < heap_array.Get_Size())
        {
            uint32_t tmp = reinterpret_cast<uint32_t>(heap_array.Get(i));

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
            Heap_Header *header = reinterpret_cast<Heap_Header *>(old_end_address);
            header->size = new_length - old_length;
            header->magic = HEAP_MAGIC;
            header->is_hole = true;

            Heap_Footer *footer = reinterpret_cast<Heap_Footer *>(old_end_address + header->size - sizeof(Heap_Footer));
            footer->header = header;
            footer->magic = HEAP_MAGIC;

            heap_array.Insert(header);
        }
        else
        {
            Heap_Header *header = reinterpret_cast<Heap_Header *>(heap_array.Get(idx));
            header->size += new_length - old_length;

            uint32_t footer_addr = reinterpret_cast<uint32_t>(header) + header->size - sizeof(Heap_Footer);

            Heap_Footer *footer = reinterpret_cast<Heap_Footer *>(footer_addr);
            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }

        return Allocate(size, should_align);
    }

    Heap_Header *orig_hole_header = reinterpret_cast<Heap_Header *>(heap_array.Get(i));
    uint32_t orig_hole_pos = reinterpret_cast<uint32_t>(orig_hole_header);
    uint32_t orig_hole_size = orig_hole_header->size;

    uint32_t leftover = orig_hole_size - new_size;
    if (leftover < sizeof(Heap_Header) + sizeof(Heap_Header))
    {
        size += leftover;
        new_size = orig_hole_size;
    }

    if (should_align && ((orig_hole_pos & 0xFFFFF000) != 0))
    {
        uint32_t size2, footer_pos;

        size2 = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(Heap_Header);

        Heap_Header *hole_header =  reinterpret_cast<Heap_Header*>(orig_hole_pos);
        hole_header->size = size2;
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = true;

        footer_pos = orig_hole_pos + size2 - sizeof(Heap_Footer);
        Heap_Footer *hole_footer = reinterpret_cast<Heap_Footer*>(footer_pos);
        hole_footer->header = hole_header;
        hole_footer->magic = HEAP_MAGIC;

        orig_hole_pos += size2;
        orig_hole_size -= size2;
    }
    else
    {
        heap_array.Remove(i);
    }

    Heap_Header *block_header = reinterpret_cast<Heap_Header *>(orig_hole_pos);
    block_header->size = new_size;
    block_header->magic = HEAP_MAGIC;
    block_header->is_hole = false;

    Heap_Footer *block_footer = reinterpret_cast<Heap_Footer *>(orig_hole_pos + sizeof(Heap_Header) + size);
    block_footer->header = block_header;
    block_footer->magic = HEAP_MAGIC;

    if (orig_hole_size - new_size > 0)
    {
        Heap_Header *hole_header = reinterpret_cast<Heap_Header *>(orig_hole_pos + sizeof(Heap_Header) + size + sizeof(Heap_Footer));
        hole_header->size = orig_hole_size - new_size;
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = true;

        Heap_Footer *hole_footer = reinterpret_cast<Heap_Footer *>(reinterpret_cast<uint32_t>(hole_header) + orig_hole_size - new_size - sizeof(Heap_Footer));

        if (reinterpret_cast<uint32_t>(hole_footer) < end_address)
        {
            hole_footer->header = hole_header;
            hole_footer->magic = HEAP_MAGIC;
        }

        heap_array.Insert(hole_header);
    }

    return reinterpret_cast<void*>(reinterpret_cast<uint32_t>(block_header) + sizeof(Heap_Header));
}

void Heap_Manager::_Free(uint32_t address)
{
    if (!reinterpret_cast<void *>(address))
        return;

    uint32_t i;

    Heap_Header *header = reinterpret_cast<Heap_Header *>(address - sizeof(Heap_Header));
    Heap_Footer *footer = reinterpret_cast<Heap_Footer *>(reinterpret_cast<uint32_t>(header) + header->size - sizeof(Heap_Footer));

    header->is_hole = true;

    bool should_add = true;

    Heap_Footer *test_footer = reinterpret_cast<Heap_Footer *>(reinterpret_cast<uint32_t>(header) - sizeof(Heap_Footer));

    if (test_footer->magic == HEAP_MAGIC && test_footer->header->is_hole)
    {
        uint32_t cache_size = header->size;

        header = test_footer->header;
        footer->header = header;

        header->size += cache_size;

        should_add = false;
    }

    Heap_Header *test_header = reinterpret_cast<Heap_Header *>(footer + sizeof(Heap_Footer));

    if (test_header->magic == HEAP_MAGIC && test_header->is_hole)
    {
        header->size += test_header->size;

        test_footer = reinterpret_cast<Heap_Footer *>(reinterpret_cast<uint32_t>(test_header) + test_header->size - sizeof(Heap_Footer));
        footer = test_footer;

        i = 0;
        while ((i < heap_array.Get_Size()) &&
               (reinterpret_cast<void *>(heap_array.Get(i)) != reinterpret_cast<void *>(test_header)))
        {
            i++;
        }

        heap_array.Remove(i);
    }

    if (reinterpret_cast<uint32_t>(footer) + sizeof(Heap_Footer) == end_address)
    {
        uint32_t old_length = end_address - start_address;
        uint32_t new_length = Contract(reinterpret_cast<uint32_t>(header) - start_address);

        if (header->size - (old_length - new_length) > 0)
        {
            header->size -= (old_length - new_length);

            footer = reinterpret_cast<Heap_Footer *>(reinterpret_cast<uint32_t>(header) + header->size - sizeof(Heap_Footer));

            footer->header = header;
            footer->magic = HEAP_MAGIC;
        }
        else
        {
            while ((i < heap_array.Get_Size()) && (reinterpret_cast<void *>(heap_array.Get(i)) != reinterpret_cast<void *>(test_header)))
            {
                i++;
            }

            if (i < heap_array.Get_Size())
            {
                heap_array.Remove(i);
            }
        }
    }

    if (should_add)
    {
        heap_array.Insert(header);
    }
}

uint32_t Heap_Manager::Malloc(uint32_t size, bool should_align, uint32_t *physical_address)
{
    if (bInitialized)
    {
        uint32_t address = Allocate(size, should_align);

        if (physical_address)
        {
            *physical_address = paging_manager->Get_Physical_Address(address);
        }

        return address;
    }
    else if (bPreInitialized)
    {
        if (should_align && (placement_address & 0x00000FFF))
        {
            placement_address &= 0xFFFFF000;
            placement_address += 0x1000;
        }

        uint32_t base = placement_address;

        if (physical_address)
            *physical_address = base;

        placement_address += size;

        return base;
    }

    return 0x0;
}

void Heap_Manager::Free(uint32_t address){
    if(bInitialized){
        _Free(address);
    }
}