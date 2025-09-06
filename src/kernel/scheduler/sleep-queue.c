/// -------------------
/// @file sleep-queue.h

#include <kernel/scheduler/sleep-queue.h>

static inline void swap(task_t **a, task_t **b)
{
    task_t *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapify_up(sleep_queue_t *heap, uint32_t idx)
{
    while (idx > 0)
    {
        uint32_t parent = (idx - 1) / 2;
        if (heap->data[parent]->wake_tick <= heap->data[idx]->wake_tick)
            break;
        swap(&heap->data[parent], &heap->data[idx]);
        idx = parent;
    }
}

static void heapify_down(sleep_queue_t *heap, uint32_t idx)
{
    while (1)
    {
        uint32_t left = idx * 2 + 1;
        uint32_t right = idx * 2 + 2;
        uint32_t smallest = idx;

        if (left < heap->size &&
            heap->data[left]->wake_tick < heap->data[smallest]->wake_tick)
        {
            smallest = left;
        }
        if (right < heap->size &&
            heap->data[right]->wake_tick < heap->data[smallest]->wake_tick)
        {
            smallest = right;
        }
        if (smallest == idx)
            break;

        swap(&heap->data[idx], &heap->data[smallest]);
        idx = smallest;
    }
}

void sleep_queue_init(sleep_queue_t *heap, void *data, uint32_t capacity)
{
    heap->data = data;
    heap->size = 0;
    heap->capacity = capacity;

    spinlock_init(&heap->lock);
}

void sleep_queue_push(sleep_queue_t *heap, task_t *task)
{
    spinlock_lock(&heap->lock);
    if (heap->size == heap->capacity)
    {
        spinlock_unlock(&heap->lock);
        return;
    }
    heap->data[heap->size] = task;
    heapify_up(heap, heap->size);
    heap->size++;
    spinlock_unlock(&heap->lock);
}

task_t *sleep_queue_peek(sleep_queue_t *heap)
{
    if (heap->size == 0)
        return NULL;
    return heap->data[0];
}

task_t *sleep_queue_pop(sleep_queue_t *heap)
{
    spinlock_lock(&heap->lock);
    if (heap->size == 0)
    {
        spinlock_unlock(&heap->lock);
        return NULL;
    }
    task_t *root = heap->data[0];
    heap->size--;
    if (heap->size > 0)
    {
        heap->data[0] = heap->data[heap->size];
        heapify_down(heap, 0);
    }
    spinlock_unlock(&heap->lock);
    return root;
}