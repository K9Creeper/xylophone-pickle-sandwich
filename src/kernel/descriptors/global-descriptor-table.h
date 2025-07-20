/// -------------------------
/// global-descriptor-table.h
/// @brief This file declares the functions for setting up the Global Descriptor Table within the kernel.

#ifndef KERNEL_DESCRIPTORS_GLOBAL_DESCRIPTOR_TABLE_H
#define KERNEL_DESCRIPTORS_GLOBAL_DESCRIPTOR_TABLE_H

#define KERNEL_DESCRIPTORS_GDT_ENTRYCOUT 8

extern void kernel_global_descriptor_table_init(void);
extern void kernel_global_descriptor_table_install(void);

#endif