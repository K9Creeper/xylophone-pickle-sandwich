/// ------------
/// scheduling.h
/// @brief This file declares the functions to handling scheduling.

#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <data-structures/scheduling/task.h>

#define MAX_TASK_COUNT 256

extern bool scheduling_is_initialized;

extern void scheduling_init(void);

extern void scheduling_install(void);

extern void scheduling_add_task(task_t* task);

extern void scheduling_exit_task(void);

extern void scheduling_create_task(const char *name, void *routine, bool is_kernel, bool is_thread);

#endif