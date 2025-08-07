/// ---------
/// syscall.h
/// @brief This file defines the enums of syscall names and declares the function handling syscalls.

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <data-structures/kernel-interrupts/kernel-interrupts.h>

typedef enum syscall_e{
    SYSCALL_EXIT = 0,
    SYSCALL_PRINT = 3,
    SYSCALL_GET_TASK_ID = 5,
    SYSCALL_GET_TASK_INFO
}syscall_t;

extern void syscall_init(void);

#endif