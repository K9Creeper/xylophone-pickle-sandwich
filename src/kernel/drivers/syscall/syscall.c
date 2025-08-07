/// ---------
/// syscall.c
/// @brief This file defines functions handling syscalls.

#include "syscall.h"

#include <memory.h>
#include <registers.h>
#include <data-structures/scheduling/task.h>

#include "../../interrupts/interrupt-service.h"

#include <stdio.h>

#define NUM_OF_SYSCALLS 1024

void* syscall_handlers[NUM_OF_SYSCALLS];

extern void _handle_syscall_interrupt(registers_t* frame);

void syscall_init(void){
    memset((uint8_t*)syscall_handlers, 0, (sizeof(void*) * NUM_OF_SYSCALLS));
    
    kernel_interrupt_service_set_handle(0x80, _handle_syscall_interrupt);
}