/// ----------------------------------
/// @file programable-interval-timer.h

#ifndef PROGRAMABLE_INTERVAL_TIMER_H
#define PROGRAMABLE_INTERVAL_TIMER_H

#include <stdint.h>
#include <data-structures/registers.h>

typedef int (*programable_interval_timer_handle_t)(registers_t*, uint32_t);

#endif