/// ---------------
/// @file kthread.h

#ifndef KTHREAD_H
#define KTHREAD_H

typedef void(*kthread_entry_t)(void);

typedef struct kthread_s{
    char* name;
    kthread_entry_t entry;
} kthread_t;

#define REGISTER_KTHREAD(name, func) \
    static const kthread_t __syscall_##func \
    __attribute__((used, section(".kthread"))) = { name, func };

#endif