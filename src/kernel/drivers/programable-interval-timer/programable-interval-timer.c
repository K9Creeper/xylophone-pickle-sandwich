/// ----------------------------------
/// @file programable-interval-timer.c

#include "programable-interval-timer.h"

#include <kernel/util.h>

#include <stddef.h>

#include <memory.h>

#include "../../interrupts/interrupts.h"

static uint8_t is_initialized;

static uint16_t hz_frequency;
static volatile uint32_t ticks = 0;

static programable_interval_timer_handle_t s_handle = NULL;

// Tick count slows down under load?
static void timer_handle(registers_t *regs)
{
    ticks++;

    IR_EOI(32);

    if (s_handle != NULL)
    {
        s_handle(regs, ticks);
    }
}

void programable_interval_timer_init(uint16_t hz)
{
    if (is_initialized)
        return;

    programable_interval_timer_set_frequency(hz);
    ticks = 0;
    kernel_interrupts_add_request_handle(0, timer_handle);
    is_initialized = 1;
}

void programable_interval_timer_set_frequency(uint16_t hz)
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

void programable_interval_timer_add_handle(programable_interval_timer_handle_t handle)
{
    s_handle = handle;
}

void programable_interval_timer_remove_handle(void)
{
    s_handle = NULL;
}

uint32_t programable_interval_timer_get_tick(void)
{
    return ticks;
}
REGISTER_SYSCALL(SYSCALLS_TICK, programable_interval_timer_get_tick);

uint32_t programable_interval_timer_get_hz(void)
{
    return hz_frequency;
}