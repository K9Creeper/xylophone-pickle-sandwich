/// ------------------
/// @file task-queue.c

#include <kernel/scheduler/task-queue.h>

void task_queue_init(task_queue_t *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    
    spinlock_init(&queue->lock);
}

void task_queue_push(task_queue_t *queue, task_t *task)
{
    spinlock_lock(&queue->lock);
    task->next = NULL;
    if (queue->tail)
    {
        queue->tail->next = task;
    }
    else
    {
        queue->head = task;
    }
    queue->tail = task;
    queue->size++;
    spinlock_unlock(&queue->lock);
}

task_t *task_queue_peek(task_queue_t *queue)
{
    if (queue->size == 0)
        return NULL;
    return queue->head;
}

task_t *task_queue_pop(task_queue_t *queue)
{
    spinlock_lock(&queue->lock);
    if (!queue->head)
    {
        spinlock_unlock(&queue->lock);
        return NULL;
    }
    task_t *task = queue->head;
    queue->head = task->next;
    if (!queue->head)
    {
        queue->tail = NULL;
    }
    task->next = NULL;
    queue->size--;
    spinlock_unlock(&queue->lock);
    return task;
}