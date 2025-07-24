/// -----
/// pit.c
/// @brief This file defines the functions of the system timer.

#include "pit.h"

#include <stddef.h>
#include <memory.h>

#include "../../interrupts/interrupt-request.h"

static pit_t system_pit;

static void* handles[PIT_HANDLE_MAX_COUNT];

static void timer_handle(registers_t* regs){
    system_pit.ticks++;

    for(uint8_t i = 0; i < PIT_HANDLE_MAX_COUNT; i++)
    {
        if(handles[i]){
            ((pit_handle_t)(handles[i]))(regs, system_pit.ticks);
        }
    }
}

void pit_init(uint16_t hz){
    if(system_pit.is_initialized)
        return;
    
    pit_set_frequency(hz);
    system_pit.ticks = 0;
    kernel_interrupt_request_set_handle(0, timer_handle);
    system_pit.is_initialized = true;
}

void pit_set_frequency(uint16_t hz){
    system_pit.hz_frequency = hz;

    uint16_t d = 1193180 / hz;

    outportb(0x43, 0x36);
    outportb(0x40, d & 0xFF);
    outportb(0x40, (d >> 8) & 0xFF);
}

int pit_add_handle(pit_handle_t handle){
    if(!system_pit.is_initialized)
        return -1;

    for(uint8_t i = 0; i < 16; i++)
        if(!handles[i]){
            handles[i] = (void*)handle;
            return i;
        }
    return -1;
}

void pit_remove_handle(int idx){
    if(!system_pit.is_initialized)
        return;
        
    if(idx >= 0 && idx < PIT_HANDLE_MAX_COUNT)
        handles[idx] = NULL; 
}