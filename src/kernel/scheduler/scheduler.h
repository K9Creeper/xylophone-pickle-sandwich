/// -----------------
/// @file scheduler.h

#ifndef KERNEL_SCHEDULER_H
#define KERNEL_SCHEDULER_H

#include <data-structures/registers.h>

#include <kernel/data-structures/scheduler/scheduler.h>
#include <kernel/data-structures/scheduler/task.h>

extern void scheduler_init(void);

extern int scheduler_schedule(registers_t *__reg_ /*unused*/, uint32_t tick);

extern int scheduler_add(task_t *task);
extern int scheduler_yield(void);
extern int scheduler_sleep(uint32_t ms);
extern task_t *scheduler_consume(void);
extern int scheduler_exit(void);

#endif