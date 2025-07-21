/// -----
/// pit.h
/// @brief This file declares the structures and functions for the system timer.

#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <data-structures/drivers/pit.h>

#define PIT_HANDLE_MAX_COUNT 16

extern void pit_init(uint16_t hz);

extern void pit_set_frequency(uint16_t hz);

extern int pit_add_handle(pit_handle_t handle);

extern void pit_remove_handle(int idx);

#endif