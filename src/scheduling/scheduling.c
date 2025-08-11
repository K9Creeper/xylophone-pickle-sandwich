/// ------------
/// scheduling.c
/// @brief This file defines the core functions of system wide scheduling.

#include <scheduling/scheduling.h>
#include <data-structures/scheduling/task.h>
#include <scheduling/task.h>
#include <scheduling/task-queue.h>
#include <scheduling/task-cleaner.h>
#include <mini-heap.h>

#include <kernel/util.h>
#include "../kernel/drivers/pit/pit.h"

#include <stdio.h>

extern void kernel_task_state_segment_set_stack(uint32_t kss, uint32_t kesp);

extern void _task_restore_context(task_t *);
extern void _task_save_context(task_t *);

static task_queue_t ready_queue;
static task_queue_t priority_ready_queue;
static mini_heap_t sleep_queue;
static task_queue_t blocked_queue;
static task_queue_t zombie_queue;

#define MAX_SLEEP_TASKS 128
static task_t *sleep_array_storage[MAX_SLEEP_TASKS];

static bool sleep_queue_is_less_than(const type_t a, const type_t b)
{
    return (((task_t *)(a))->sleep < ((task_t *)(b))->sleep);
}

task_t *system_current_task = &(task_t){
    .name = "kernel",
    .pid = 0,
};

static task_t *current_task = NULL;

static bool is_initalized = false;

static int round_robin(uint32_t tick);

void scheduling_init(void)
{
    if (is_initalized)
        return;

    task_init();

    task_queue_init(&priority_ready_queue);
    task_queue_init(&ready_queue);
    task_queue_init(&blocked_queue);
    task_queue_init(&zombie_queue);

    mini_heap_init(&sleep_queue, (type_t *)sleep_array_storage, MAX_SLEEP_TASKS, sleep_queue_is_less_than);

    is_initalized = true;
}

static inline void enqueue_ready(task_t *task)
{
    if (task->state != TASK_STATE_CREATED)
        task->state = TASK_STATE_RUNNING;
    if (task->is_priority)
        task_queue_push(&priority_ready_queue, task);
    else
        task_queue_push(&ready_queue, task);
}

static inline void enqueue_sleep(task_t *task)
{
    task->state = TASK_STATE_SLEEPING;
    mini_heap_insert(&sleep_queue, task);
}

static inline void enqueue_blocked(task_t *task)
{
    task->state = TASK_STATE_BLOCKED;
    task_queue_push(&blocked_queue, task);
}

static inline void enqueue_zombie(task_t *task)
{
    task->state = TASK_STATE_ZOMBIE;
    task_queue_push(&zombie_queue, task);
}

static inline task_t *get_next_ready(void)
{
    task_t *t = task_queue_pop(&priority_ready_queue);
    if (!t)
        t = task_queue_pop(&ready_queue);
    return t;
}

static void wake_sleepers(uint32_t tick)
{
    while (sleep_queue.size > 0)
    {
        task_t *t = mini_heap_peek(&sleep_queue);
        if (!t || t->sleep > tick)
            break;

        mini_heap_pop(&sleep_queue);
        enqueue_ready(t);
    }
}
static int round_robin(uint32_t tick)
{
    task_t *next;

    wake_sleepers(tick);

    if (current_task && current_task->state == TASK_STATE_RUNNING)
    {
        enqueue_ready(current_task);
        current_task = NULL;
    }

    next = get_next_ready();
    if (!next)
    {
        printf("Scheduler: No runnable tasks!\n");
        PANIC();
        return -1;
    }

    if (next->state == TASK_STATE_CREATED)
    {
        if (next->mode != TASK_MODE_KTHREAD)
            kernel_task_state_segment_set_stack(0x10, next->kebp);

        if (current_task == NULL || current_task->physical_cr3 != next->physical_cr3)
            paging_manager_swap_system_page_directory(next->physical_cr3, true);

        current_task = next;
        system_current_task = next;

        task_start(next);
    }

    while (next->state != TASK_STATE_RUNNING)
    {
        switch (next->state)
        {
        case TASK_STATE_ZOMBIE:
            enqueue_zombie(next);
            break;

        case TASK_STATE_SLEEPING:
            enqueue_sleep(next);
            break;

        case TASK_STATE_BLOCKED:
            enqueue_blocked(next);
            break;

        case TASK_STATE_CREATED:
            if (next->mode != TASK_MODE_KTHREAD)
                kernel_task_state_segment_set_stack(0x10, next->kebp);

            if (current_task == NULL || current_task->physical_cr3 != next->physical_cr3)
                paging_manager_swap_system_page_directory(next->physical_cr3, true);

            current_task = next;
            system_current_task = next;
            task_start(next);
            break;

        default:
            enqueue_ready(next);
            break;
        }

        next = get_next_ready();
        if (!next)
        {
            printf("Scheduler: No runnable tasks after filtering!\n");
            PANIC();
            return -1;
        }
    }

    if (current_task != next)
    {
        if (next->mode != TASK_MODE_KTHREAD)
            kernel_task_state_segment_set_stack(0x10, next->kebp);

        if (current_task == NULL || current_task->physical_cr3 != next->physical_cr3)
            paging_manager_swap_system_page_directory(next->physical_cr3, true);

        current_task = next;
        system_current_task = next;
    }

    return 0;
}

void scheduling_maintenance(void)
{
    while (!task_queue_empty(&zombie_queue))
    {
        task_t *z = task_queue_pop(&zombie_queue);
        if (!z)
            continue;

        if (task_cleaner_add(z) != 0)
        {
            task_queue_push(&zombie_queue, z);
            break;
        }
    }
}

int scheduling_exit(void)
{
    if (current_task == NULL)
        return -1;

    current_task->state = TASK_STATE_ZOMBIE;

    INTERRUPT_SAFE_BLOCK({
        _task_save_context(current_task);

        round_robin(pit_get_tick());

        _task_restore_context(current_task);
    });

    return 0;
}
extern void test(registers_t *d, uint32_t t);
int scheduling_schedule(registers_t *__reg_ /*unused*/, uint32_t tick)
{
    if (__reg_ != NULL)
        test(NULL, tick);

    if (current_task == NULL)
    {
        current_task = task_queue_pop(&priority_ready_queue);
        if (!current_task)
            current_task = task_queue_pop(&ready_queue);
        system_current_task = current_task;
    }

    {
        INTERRUPT_SAFE_BLOCK({
            _task_save_context(current_task);

            round_robin(tick);

            _task_restore_context(current_task);
        });
    }

    return 0;
}

int scheduling_prioritize(task_t *task)
{
    if (task == NULL)
        return -1;

    task->is_priority = true;
    enqueue_ready(task);

    return 0;
}

int scheduling_add(task_t *task)
{
    if (task == NULL)
        return -1;

    task->is_priority = false;
    enqueue_ready(task);

    return 0;
}

int scheduling_block(task_t *task)
{
    if (task == NULL)
        return -1;

    enqueue_blocked(task);

    INTERRUPT_SAFE_BLOCK({
        _task_save_context(task);
        round_robin(pit_get_tick());
        _task_restore_context(current_task);
    });

    return 0;
}

int scheduling_sleep(int ms)
{
    uint32_t tick_now = pit_get_tick();
    uint32_t hz = pit_get_hz();

    uint32_t ticks_to_sleep = (ms * hz) / 1000;
    if (ticks_to_sleep == 0)
        ticks_to_sleep = 1;

    current_task->sleep = tick_now + ticks_to_sleep;
    enqueue_sleep(current_task);

    scheduling_schedule(NULL, tick_now);

    return 0;
}

task_t *scheduling_consume(void)
{
    task_t *task = current_task;
    current_task = NULL;
    return task;
}

// Wrapper
int scheduling_yield(void)
{
    return scheduling_schedule(NULL, pit_get_tick());
}

// Syscall for current thread/task
void *syscall_malloc(int size)
{
    void *ret = NULL;
    INTERRUPT_SAFE_BLOCK({
        if (current_task == NULL)
            break;

        ret = heap_manager_malloc(current_task->heap_manger, size, false, NULL);
    });
    return ret;
}

void syscall_free(void *address)
{
    INTERRUPT_SAFE_BLOCK({
        if (current_task == NULL)
            break;
        heap_manager_free(current_task->heap_manger, (uint32_t)address);
    });
}