/// ------------
/// task-queue.h
/// @brief This file defines the structure for task-queue.

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "task.h"

typedef struct task_queue_s {
	task_t* _list;
	int total;
} task_queue_t;

#endif