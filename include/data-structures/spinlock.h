/// ----------
/// spinlock.h
/// @brief This file defines the strucuture of a spinlock.

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>
#include <stdatomic.h>

typedef _Atomic bool spinlock_t;

#endif