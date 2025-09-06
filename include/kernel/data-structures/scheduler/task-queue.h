/// ------------------
/// @file task-queue.h

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <kernel/data-structures/scheduler/task.h>
#include <spinlock.h>

typedef struct task_queue_s{
    task_t* head;
    task_t* tail;
    uint32_t size;

    spinlock_t lock;
}task_queue_t;

#endif