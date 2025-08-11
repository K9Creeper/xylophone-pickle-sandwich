/// ------
/// task.h
/// @brief This file declates the functions for handling tasks.

#ifndef SCHEDULING_TASK_H
#define SCHEDULING_TASK_H

#include <data-structures/scheduling/task.h>

extern void task_init(void);

extern void task_start(task_t* task);

extern int task_create_kthread(task_entry_routine_t entry, char* name, int argc, char** argv);
extern int task_create_thread(task_t* parent, task_entry_routine_t entry, void* arg, bool is_kernel);

#endif