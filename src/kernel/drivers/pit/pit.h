/// -----
/// pit.h
/// @brief This file declares the structures and functions for the system timer.

#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <data-structures/drivers/pit.h>

extern void pit_init(uint16_t hz);

extern void pit_set_frequency(uint16_t hz);

extern void pit_add_handle(pit_handle_t handle);

extern void pit_remove_handle(void);

extern uint32_t pit_get_tick(void);

#endif