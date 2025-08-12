/// ---------
/// syscall.h
/// @brief This file defines the structure related to syscalls.

#ifndef SYSCALL_STRUCTURE_H
#define SYSCALL_STRUCTURE_H

typedef enum syscall_e{
    SYSCALL_EXIT,
    SYSCALL_SLEEP,
    SYSCALL_YIELD,
    SYSCALL_MALLOC,
    SYSCALL_FREE
}syscall_t;

#endif 