/// ---------
/// kthread.h
/// @brief This file definess the structures for handling kernel threads.

#ifndef _KTHREAD_H
#define _KTHREAD_H

#include <data-structures/scheduling/task.h>

typedef void(*kthread_entry_t)(void);

typedef struct kthread_s{
    char name[TASK_MAX_NAME_LENGTH];
    kthread_entry_t entry;
} kthread_t;

#endif