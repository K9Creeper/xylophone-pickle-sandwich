/// ---------------
/// @file kthread.h

#ifndef KTHREAD_H
#define KTHREAD_H

#include <kernel/data-structures/scheduler/task.h>

#define KTHREAD_MAX_COUNT 128

typedef void(*kthread_entry_t)(void);

typedef struct kthread_s{
    char name[TASK_NAME_LENGTH];
    kthread_entry_t entry;
} kthread_t;

#define REGISTER_KTHREAD(num, func) \
    static const kthread_t __syscall_##func \
    __attribute__((used, section(".kthread"))) = { num, func };

#endif