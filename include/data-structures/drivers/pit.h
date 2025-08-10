/// ------
/// pit.h
/// @brief This file declares the structures for the system timer.

#ifndef PIT_STRUCTURE_H
#define PIT_STRUCTURE_H

#include <stdint.h>
#include <stdbool.h>
#include <registers.h>

typedef int (*pit_handle_t)(registers_t*, uint32_t);

typedef struct pit_s{
    bool is_initialized;

    uint16_t hz_frequency;
    volatile uint32_t ticks;
}pit_t;

#endif