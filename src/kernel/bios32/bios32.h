/// --------
/// bios32.h
/// @brief This file declares the functions to use bios calls from x86 protected mode.

#ifndef BIOS32_H
#define BIOS32_H

#include <registers.h>
#include <stdbool.h>

extern void kernel_bios32_init(void);
extern void kernel_bios32_call(uint8_t int_num, registers16_t * in_reg, registers16_t* out_reg, bool enable_int);

#endif