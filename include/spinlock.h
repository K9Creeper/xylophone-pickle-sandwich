/// ----------
/// spinlock.h
/// @brief This file declares the functions that handle spinlocks.

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <data-structures/spinlock.h>

extern void spinlock_lock(spinlock_t* spinlock);
extern void spinlock_unlock(spinlock_t* spinlock);

#endif