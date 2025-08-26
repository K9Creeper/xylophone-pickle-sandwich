/// -------------------------------
/// @file global-descriptor-table.h

#ifndef KERNEL_DESCRIPTORS_GLOBAL_DESCRIPTOR_TABLE_H
#define KERNEL_DESCRIPTORS_GLOBAL_DESCRIPTOR_TABLE_H

#define KERNEL_DESCRIPTORS_GDT_ENTRYCOUT 8

extern void kernel_global_descriptor_table_init(void);
extern void kernel_global_descriptor_table_install(void);

#endif