/// ------------------
/// @file interrupts.h

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <data-structures/registers.h>

typedef void(*interrupts_handle_t)(registers_t* __regs__);

#endif