/// ----------------------------
/// interrupt-descriptor-table.c
/// @brief This file defines core functions to setup the Interrupt Descriptor Table, 
/// aswell as setting up all interrupt gates.

#include "interrupt-descriptor-table.h"
#include <data-structures/kernel-descriptors/interrupt-descriptor-table.h>

#include <memory.h>

interrupt_descriptor_table_entry_t pIDT[KERNEL_DESCRIPTORS_IDT_ENTRYCOUT];
interrupt_descriptor_table_ptr_t _pIDT;

// interrupt-descriptor-table.s
extern void _idt_load(void);

// ../interrupts/interrupt-request.c
extern void kernel_interrupt_request_remap(void);
extern void kernel_interrupt_request_add_gates(void);

// ../interrupts/interrupt-service.c
extern void kernel_interrupt_service_add_gates(void);

void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

void kernel_interrupt_descriptor_table_init(void){
    _pIDT.limit = (sizeof(interrupt_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_IDT_ENTRYCOUT) - 1;
    _pIDT.base = (unsigned int)(&pIDT);
    memset((unsigned char *)(&pIDT), 0, sizeof(interrupt_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_IDT_ENTRYCOUT);
}

void kernel_interrupt_descriptor_table_install(void){
    kernel_interrupt_request_remap();
    kernel_interrupt_service_add_gates();
    kernel_interrupt_request_add_gates();

    _idt_load();
}

void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags){
    pIDT[num].base_high = (base >> 16) & 0xFFFF;
    pIDT[num].base_low = (base & 0xFFFF);
    pIDT[num].always0 = 0;
    pIDT[num].selector = selector;
    pIDT[num].flags = flags;
}