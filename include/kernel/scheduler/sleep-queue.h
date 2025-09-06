/// -------------------
/// @file sleep-queue.h

#ifndef _SLEEP_QUEUE_H
#define _SLEEP_QUEUE_H

#include <kernel/data-structures/scheduler/sleep-queue.h>

extern void sleep_queue_init(sleep_queue_t* heap, void* data, uint32_t capacity);

extern void sleep_queue_push(sleep_queue_t* heap, task_t* task);
extern task_t* sleep_queue_peek(sleep_queue_t* heap);
extern task_t* sleep_queue_pop(sleep_queue_t* heap);

#endif