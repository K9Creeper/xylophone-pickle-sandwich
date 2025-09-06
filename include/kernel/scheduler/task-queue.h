/// ------------------
/// @file task-queue.h

#ifndef _TASK_QUEUE_H
#define _TASK_QUEUE_H

#include <kernel/data-structures/scheduler/task-queue.h>

extern void task_queue_init(task_queue_t* queue);

extern void task_queue_push(task_queue_t* queue, task_t* task);
extern task_t* task_queue_peek(task_queue_t* queue);
extern task_t* task_queue_pop(task_queue_t* queue);

#endif