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
    SYSCALL_FREE,
    SYSCALL_GET_SYSTEM_TICK_COUNT,
    SYSCALL_GET_TASK_DIRECTORY
}syscall_t;

#endif 