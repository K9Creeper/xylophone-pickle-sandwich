/// ----------
/// spinlock.c
/// @brief This file defines the functions of the spinlock.

#include <spinlock.h>

void spinlock_lock(spinlock_t* spinlock){
    while (atomic_exchange_explicit(spinlock, true, memory_order_acquire)) {
        __builtin_ia32_pause();
    }
}

void spinlock_unlock(spinlock_t* spinlock){
    atomic_store_explicit(spinlock, false, memory_order_release);
}