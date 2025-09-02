/// --------------
/// @file bios32.h

#ifndef KERNEL_BIOS32_H
#define KERNEL_BIOS32_H

#include <stdint.h>

#include <data-structures/registers.h>

extern uint8_t kernel_bios32_init(void);

extern void* kernel_bios32_service(uint8_t interrupt_num, registers16_t *in_reg, registers16_t *out_reg);

#endif