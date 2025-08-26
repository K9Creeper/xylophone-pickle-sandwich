/// ------------------
/// @file multiboot2.h

#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <multiboot2.h>
#include <stdint.h>

extern void multiboot2_get_physical_memory_size(uint32_t addr, uint32_t* memory);

#define IS_VALID_MULTIBOOT2_MAGIC(magic_number) magic_number == MULTIBOOT2_BOOTLOADER_MAGIC

#endif