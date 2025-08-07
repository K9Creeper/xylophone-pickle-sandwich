/// -----
/// pit.c
/// @brief This file defines the functions of the system timer.

#include "pit.h"

#include <stddef.h>
#include <memory.h>

#include "../../interrupts/interrupt-request.h"

static pit_t system_pit;

static void* s_handle;

static void timer_handle(registers_t* regs){
    system_pit.ticks++;


    if(s_handle != NULL){
        if((((pit_handle_t)(s_handle))(regs, system_pit.ticks)))
        {
                // Bad..
        }
    }
}

void pit_init(uint16_t hz){
    if(system_pit.is_initialized)
        return;
    
    s_handle = NULL;

    pit_set_frequency(hz);
    system_pit.ticks = 0;
    kernel_interrupt_request_set_handle(0, timer_handle);
    system_pit.is_initialized = true;
}

void pit_set_frequency(uint16_t hz){
    system_pit.hz_frequency = hz;

    uint32_t d = 1193180U / hz;

    outportb(0x43, 0x36);

    outportb(0x40, (uint8_t)(d & 0xFF));
    outportb(0x40, (uint8_t)((d >> 8) & 0xFF));
}

void pit_add_handle(pit_handle_t handle){
    if(!system_pit.is_initialized)
        return;

    s_handle = (void*)handle;
}

void pit_remove_handle(void){
    if(!system_pit.is_initialized)
        return;
        
    s_handle = NULL;
}

uint32_t pit_get_tick(void){
    return system_pit.ticks;
}