/// ---------------
/// @file kthread.h

#ifndef KTHREAD_H
#define KTHREAD_H

#define KTHREAD_MAX_COUNT 128

typedef void(*kthread_entry_t)(void);

typedef struct kthread_s{
    char name[32];
    kthread_entry_t entry;
} kthread_t;

#define REGISTER_KTHREAD(num, func) \
    static const kthread_t __syscall_##func \
    __attribute__((used, section(".kthread"))) = { num, func };

#endif