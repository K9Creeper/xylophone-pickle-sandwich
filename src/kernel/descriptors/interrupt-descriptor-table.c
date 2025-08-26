/// ----------------------------------
/// @file interrupt-descriptor-table.c

#include "interrupt-descriptor-table.h"
#include <kernel/data-structures/descriptors/interrupt-descriptor-table.h>

#include <memory.h>

interrupt_descriptor_table_entry_t pIDT[KERNEL_DESCRIPTORS_IDT_ENTRYCOUT];
interrupt_descriptor_table_ptr_t _pIDT;

// interrupt-descriptor-table.s
extern void _idt_load(void);

void kernel_interrupt_descriptor_table_init(void)
{
    _pIDT.limit = (sizeof(interrupt_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_IDT_ENTRYCOUT) - 1;
    _pIDT.base = (unsigned int)(&pIDT);
    memset((unsigned char *)(pIDT), 0, sizeof(interrupt_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_IDT_ENTRYCOUT);
}

void kernel_interrupt_descriptor_table_finalize(void){
    _idt_load();
}

void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    pIDT[num].base_high = (base >> 16) & 0xFFFF;
    pIDT[num].base_low = (base & 0xFFFF);
    pIDT[num].always0 = 0;
    pIDT[num].selector = selector;
    pIDT[num].flags = flags;
}