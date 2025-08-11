/// ------------
/// task-queue.c
/// @brief This file defines the functions of the task queue handling.

#include <scheduling/task-queue.h>
#include <spinlock.h>

void task_queue_init(task_queue_t *queue)
{
    if (queue == NULL)
        return;

    queue->_list = NULL;
    queue->total = 0;
    queue->spinlock = false;
}

int task_queue_push(task_queue_t *queue, task_t *task)
{
    if (queue == NULL || task == NULL)
        return -1;

    //spinlock_lock(&queue->spinlock);
    
    task_t *current = queue->_list;
    if (current == NULL)
    {
        queue->_list = task;
        //spinlock_unlock(&queue->spinlock);
        return 0;
    }
    while (current->next != NULL)
    {
        current = current->next;
    }
    current->next = task;
    task->next = NULL;
    queue->total++;
    
    //spinlock_unlock(&queue->spinlock);

    return 0;
}

int task_queue_add(task_queue_t *queue, task_t *task)
{
    if (queue == NULL)
        return -1;

    //spinlock_lock(&queue->spinlock);

    task->next = queue->_list;
    queue->_list = task;

    queue->total++;

    //spinlock_unlock(&queue->spinlock);

    return 0;
}

void task_queue_remove(task_queue_t *queue, task_t *task)
{
    if (queue == NULL || task == NULL || queue->_list == NULL)
        return;

    //spinlock_lock(&queue->spinlock);

    if (queue->_list == task)
    {
        queue->_list = task->next;
        queue->total--;
        //spinlock_unlock(&queue->spinlock);
        return;
    }

    task_t *current = queue->_list;
    while (current->next != NULL && current->next != task)
    {
        current = current->next;
    }

    if (current->next == NULL)
    {
        //spinlock_unlock(&queue->spinlock);
        return;
    }

    current->next = task->next;
    queue->total--;

    //spinlock_unlock(&queue->spinlock);
}

task_t *task_queue_pop(task_queue_t *queue)
{
    if (queue == NULL || queue->_list == NULL)
        return NULL;

    //spinlock_lock(&queue->spinlock);

    task_t *front = NULL;

    front = queue->_list;
    queue->_list = front->next;

    front->next = NULL;
    front->prev = NULL;

    //spinlock_unlock(&queue->spinlock);

    return front;
}

task_t *task_queue_peek(task_queue_t *queue)
{
    if (queue == NULL || queue->_list == NULL)
        return NULL;

    //spinlock_lock(&queue->spinlock);

    task_t *front = NULL;

    front = queue->_list;

    //spinlock_unlock(&queue->spinlock);

    return front;
}

bool task_queue_empty(task_queue_t* queue){
    return queue->_list == NULL;
}