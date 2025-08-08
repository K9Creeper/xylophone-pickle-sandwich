/// --------
/// bios32.h
/// @brief This file declares the functions and structures for BIOS32 services.

#ifndef BIOS32_H
#define BIOS32_H

#include <registers.h>

extern void bios32_init(void);
extern void bios32_service(uint8_t interrupt_num, registers16_t * in_reg, registers16_t * out_reg);

#endif