/// ------
/// pit.h
/// @brief This file declares the structures for the system timer.

#ifndef PIT_STRUCTURE_H
#define PIT_STRUCTURE_H

#include <stdint.h>
#include <registers.h>

typedef int (*pit_handle_t)(registers_t*, uint32_t);

#endif