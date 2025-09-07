/// -------------------------------
/// @file global-descriptor-table.c

#include "global-descriptor-table.h"

#include <stdint.h>
#include <memory.h>

#include <kernel/data-structures/descriptors/global-descriptor-table.h>

#define GDT_GRANULARITY 0xCF

#define GDT_ACCESS_CODE_PL0 0x9A
#define GDT_ACCESS_DATA_PL0 0x92

#define GDT_ACCESS_CODE_PL3 0xFA
#define GDT_ACCESS_DATA_PL3 0xF2

global_descriptor_table_entry_t pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];
global_descriptor_table_ptr_t _pGDT;

// @file global-descriptor-table.s
extern void _gdt_flush(void);

void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void kernel_global_descriptor_table_init(void)
{
    _pGDT.limit = (sizeof(global_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_GDT_ENTRYCOUT) - 1;
    _pGDT.base = (uint32_t)(&pGDT);
    memset((unsigned char *)(pGDT), 0, sizeof(global_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_GDT_ENTRYCOUT);
}

void kernel_global_descriptor_table_install(void)
{
    // Null
    kernel_global_descriptor_set_gate(0, 0, 0, 0, 0);

    // kernel (32-bit)
    kernel_global_descriptor_set_gate(1, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL0, GDT_GRANULARITY);
    kernel_global_descriptor_set_gate(2, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL0, GDT_GRANULARITY);

    // User mode (32-bit)
    kernel_global_descriptor_set_gate(3, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL3, GDT_GRANULARITY);
    kernel_global_descriptor_set_gate(4, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL3, GDT_GRANULARITY);

    // Reserve index [5] for Task State Segment

    // Real mode (16-bit)
    kernel_global_descriptor_set_gate(6, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL0, 0x0F);
    kernel_global_descriptor_set_gate(7, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL0, 0x0F);

    _gdt_flush();
}

void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
{
    pGDT[index].base_low = (base & 0xFFFF);
    pGDT[index].base_middle = (base >> 16) & 0xFF;
    pGDT[index].base_high = (base >> 24) & 0xFF;

    pGDT[index].limit_low = (limit & 0xFFFF);
    pGDT[index].granularity = ((limit >> 16) & 0x0F);

    pGDT[index].granularity |= (granularity & 0xF0);
    pGDT[index].access = access;
}