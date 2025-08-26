/// ----------------------------------
/// @file programable-interval-timer.h

#ifndef DRIVERS_PROGRAMABLE_INTERVAL_TIMER_H
#define DRIVERS_PROGRAMABLE_INTERVAL_TIMER_H

#include <stdint.h>
#include <kernel/data-structures/drivers/programable-interval-timer.h>

extern void programable_interval_timer_init(uint16_t hz);

extern void programable_interval_timer_set_frequency(uint16_t hz);

extern void programable_interval_timer_add_handle(programable_interval_timer_handle_t handle);

extern void programable_interval_timer_remove_handle(void);

extern uint32_t programable_interval_timer_get_tick(void);

extern uint32_t programable_interval_timer_get_hz(void);

#endif