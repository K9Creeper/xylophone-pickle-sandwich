/// ----------------------------
/// interrupt-descriptor-table.h
/// @brief This file declares the needed function to setup the Interrupt Descriptor Table.

#ifndef KERNEL_DESCRIPTORS_INTERRUPT_DESCRIPTOR_TABLE_H
#define KERNEL_DESCRIPTORS_INTERRUPT_DESCRIPTOR_TABLE_H

#define KERNEL_DESCRIPTORS_IDT_ENTRYCOUT 256

extern void kernel_interrupt_descriptor_table_init(void);
extern void kernel_interrupt_descriptor_table_install(void);

#endif