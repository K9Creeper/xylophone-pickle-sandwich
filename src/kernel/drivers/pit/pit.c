/// -----
/// pit.c
/// @brief This file defines the functions of the system timer.

#include "pit.h"

#include <stdbool.h>

#include <stddef.h>
#include <memory.h>
#include <stdio.h>

#include <kernel/util.h>

#include "../../interrupts/interrupt-request.h"

static bool is_initialized;

static uint16_t hz_frequency;
static uint32_t ticks = 0;

static void *s_handle = NULL;

// Tick count slows down under load?
static void timer_handle(registers_t *regs)
{
    ticks++;

    IRQ_EOI(0);

    if (s_handle != NULL)
    {
        ((pit_handle_t)(s_handle))(regs, ticks);
    }
}

void pit_init(uint16_t hz)
{
    if (is_initialized)
        return;

    pit_set_frequency(hz);
    ticks = 0;
    kernel_interrupt_request_set_handle(0, timer_handle);
    is_initialized = true;
}

void pit_set_frequency(uint16_t hz)
{
    uint16_t divisor = 1193180 / hz;

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    INTERRUPT_SAFE_BLOCK({
        outportb(0x43, 0x36);
        outportb(0x40, l);
        outportb(0x40, h);
    });

    hz_frequency = hz;
}

void pit_add_handle(pit_handle_t handle)
{
    s_handle = (void *)handle;
}

void pit_remove_handle(void)
{
    s_handle = NULL;
}

uint32_t pit_get_tick(void)
{
    return ticks;
}

uint32_t pit_get_hz(void)
{
    return hz_frequency;
}