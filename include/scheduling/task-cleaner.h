/// --------------
/// task-cleaner.h
/// @brief This file declares the functions to cleaning ZOMBIE or exited tasks.

#ifndef TASK_CLEANER_H
#define TASK_CLEANER_H

#include <data-structures/scheduling/task.h>

extern int task_cleaner_add(task_t* task_to_clean);

#endif