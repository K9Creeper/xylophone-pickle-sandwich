/// ---------
/// syscall.h
/// @brief This file declares and defines structures and function(s) related to syscalls.

#ifndef SYSCALL_H
#define SYSCALL_H

#include <data-structures/syscall.h>

void yield(void);
void exit(void);
void sleep(int ms);

#endif