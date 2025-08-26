/// ----------------------------------
/// @file interrupt-descriptor-table.h

#ifndef KERNEL_DESCRIPTORS_INTERRUPT_DESCRIPTOR_TABLE_H
#define KERNEL_DESCRIPTORS_INTERRUPT_DESCRIPTOR_TABLE_H

#include <stdint.h>

#define KERNEL_DESCRIPTORS_IDT_ENTRYCOUT 256

extern void kernel_interrupt_descriptor_table_init(void);
extern void kernel_interrupt_descriptor_table_finalize(void);

extern void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

#endif