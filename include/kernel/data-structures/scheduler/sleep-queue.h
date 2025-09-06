/// -------------------
/// @file sleep-queue.h

#ifndef SLEEP_QUEUE_H
#define SLEEP_QUEUE_H

#include <kernel/data-structures/scheduler/task.h>
#include <spinlock.h>

typedef struct {
    task_t** data;
    uint32_t size;
    uint32_t capacity;

    spinlock_t lock;
} sleep_queue_t;

#endif