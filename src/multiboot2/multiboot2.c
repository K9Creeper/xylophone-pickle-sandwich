/// ------------------
/// @file multiboot2.c

// Refer to: https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html

#include "multiboot2.h"

#include <stddef.h>

// TODO: Make sure to only count regions above kernel (end) address.
//
void multiboot2_get_physical_memory_size(uint32_t addr, uint32_t *memory)
{
    if (memory == NULL)
        return;

    uint32_t size = *(unsigned *)addr;

    for (struct multiboot_tag *tag = (struct multiboot_tag *)(addr + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch (tag->type)
        {
        case MULTIBOOT_TAG_TYPE_MMAP:
        {
            multiboot_memory_map_t *mmap;
            
            for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
                 (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
                 mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
            {
                if(mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->addr == 0x0) continue;
                
                *memory += mmap->len;
            }
        }
            return;
        default:
            break;
        }
    }
}