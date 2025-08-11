/// ------------
/// task-queue.h
/// @brief This file declares the functions for the task queue.

#ifndef SCHEDULING_TASK_QUEUE_H
#define SCHEDULING_TASK_QUEUE_H

#include <data-structures/scheduling/task-queue.h>

extern void task_queue_init(task_queue_t* queue);

extern int task_queue_push(task_queue_t* queue, task_t* task);
extern int task_queue_add(task_queue_t* queue, task_t* task);

extern void task_queue_remove(task_queue_t* queue, task_t* task);

extern task_t* task_queue_pop(task_queue_t* queue);
extern task_t* task_queue_peek(task_queue_t* queue);

extern bool task_queue_empty(task_queue_t* queue);

#endif