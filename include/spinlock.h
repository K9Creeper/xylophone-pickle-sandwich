/// ----------------
/// @file spinlock.h

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <data-structures/spinlock.h>

static inline void spinlock_lock(spinlock_t *spinlock)
{
    while (atomic_exchange_explicit(spinlock, 1, memory_order_acquire))
    {
        __builtin_ia32_pause();
    }
}

static inline void spinlock_unlock(spinlock_t *spinlock)
{
    atomic_store_explicit(spinlock, 0, memory_order_release);
}

#endif