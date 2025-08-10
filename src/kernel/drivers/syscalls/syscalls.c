/// ---------
/// syscalls.c
/// @brief This file defines functions handling syscalls.

#include "syscalls.h"

#include <memory.h>
#include <registers.h>
#include <data-structures/scheduling/task.h>

#include "../../interrupts/interrupt-service.h"

#include <stdio.h>

#define NUM_OF_SYSCALLS 1024

void* syscalls_handlers[NUM_OF_SYSCALLS];

// syscalls.s
extern void _handle_syscalls_interrupt(registers_t* frame);

void syscalls_init(void){
    memset((uint8_t*)syscalls_handlers, 0, (sizeof(void*) * NUM_OF_SYSCALLS));
    
    kernel_interrupt_service_set_handle(0x80, _handle_syscalls_interrupt);
}

void syscalls_register(uint16_t index, void* func){
    if(index >= 0 && index < NUM_OF_SYSCALLS)
        syscalls_handlers[index] = func;   
}