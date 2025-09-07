/// ------------
/// @file task.h

#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

#include <kernel/data-structures/scheduler/task.h>

extern void task_init(void);

extern void task_start_impl(task_t *task);
extern int task_cleanup(uint16_t pid);

extern int task_create_kthread(task_entry_routine_t entry, char *name, int argc, char **argv);

#endif