/// ---------
/// syscall.h
/// @brief This file declares and defines structures and function(s) related to syscalls.

#ifndef SYSCALL_H
#define SYSCALL_H

#include <data-structures/syscall.h>

#include <stdint.h>

void yield(void);
void exit(void);
void sleep(int ms);

void* malloc(int size);
void free(void* address);

uint32_t get_system_tick_count(void);

void get_task_directory(char* buffer, uint32_t buffer_size);

#endif