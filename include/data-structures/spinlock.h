/// ----------------
/// @file spinlock.h

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>
#include <stdatomic.h>

typedef _Atomic uint8_t spinlock_t;

#endif