/// ---------
/// syscalls.h
/// @brief This file defines the enums of syscall names and declares the function handling syscalls.

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <data-structures/kernel/kernel-interrupts.h>
#include <data-structures/syscall.h>

extern void syscalls_init(void);
extern void syscalls_register(uint16_t index, void* func);

#endif