/// -------------------------
/// global-descriptor-table.c
/// @brief This file defines the functions for setting up the Global Descriptor Table within the kernel.

#include "global-descriptor-table.h"

#include <stdint.h>

#include <data-structures/kernel-descriptors/global-descriptor-table.h>

#define NULL 0
#define GDT_GRANULARITY 0xCF

#define GDT_ACCESS_CODE_PL0  0x9A
#define GDT_ACCESS_DATA_PL0  0x92

#define GDT_ACCESS_CODE_PL3  0xFA
#define GDT_ACCESS_DATA_PL3  0xF2

global_descriptor_table_entry_t pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];
global_descriptor_table_ptr_t _pGDT;

// global-descriptor-table.s
extern void _gdt_flush(void);

void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

void kernel_global_descriptor_table_init(void){
    _pGDT.limit = (sizeof(global_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_GDT_ENTRYCOUT) - 1;
    _pGDT.base = (uint32_t)(&pGDT);
}

void kernel_global_descriptor_table_install(void){
    kernel_global_descriptor_set_gate(NULL, NULL, NULL, NULL, NULL);

    // kernel (32-bit)
    kernel_global_descriptor_set_gate(1, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL0, GDT_GRANULARITY);
    kernel_global_descriptor_set_gate(2, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL0, GDT_GRANULARITY);

    // User mode (32-bit)
    kernel_global_descriptor_set_gate(3, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_PL3, GDT_GRANULARITY); 
    kernel_global_descriptor_set_gate(4, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_PL3, GDT_GRANULARITY);

    _gdt_flush();
}

void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity){
    pGDT[index].base_low = (base & 0xFFFF);
    pGDT[index].base_middle = (base >> 16) & 0xFF;
    pGDT[index].base_high = (base >> 24) & 0xFF;

    pGDT[index].limit_low = (limit & 0xFFFF);
    pGDT[index].granularity = ((limit >> 16) & 0x0F);

    pGDT[index].granularity |= (granularity & 0xF0);
    pGDT[index].access = access;
}