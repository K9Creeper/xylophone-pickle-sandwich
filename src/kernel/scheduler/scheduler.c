#include "scheduler.h"

#include <kernel/util.h>

#include <kernel/memory-management/heap-manager.h>
#include <kernel/memory-management/paging-manager.h>
#include <kernel/memory-management/allocation-table.h>

#include <kernel/scheduler/task-queue.h>
#include <kernel/scheduler/sleep-queue.h>

#include "../descriptors/task-state-segment.h"
#include "../drivers/programable-interval-timer/programable-interval-timer.h"

#include "kernel/data-structures/kthread/kthread.h"
#include "../kthread/kthread.h"

#include "task.h"

static task_t *current_tasks[MAX_SCHEDULER_CPUS];

static inline task_t *current_task(void)
{
    return current_tasks[0];
}

static inline void set_current_task(task_t *task)
{
    current_tasks[0] = task;
}

static task_queue_t ready_queue;
#define MAX_SLEEP_TASKS 128
static task_t *sleep_array_storage[MAX_SLEEP_TASKS];
static sleep_queue_t sleep_queue;
static task_queue_t zombie_queue;

static uint8_t is_initalized = 0;

extern void _task_restore_context(task_t *);
extern void _task_save_context(task_t *);

#include <kernel/dbgprintf.h>

void scheduler_init(void)
{
    if (is_initalized)
        return;

    task_init();

    task_queue_init(&ready_queue);
    task_queue_init(&zombie_queue);

    sleep_queue_init(&sleep_queue, (task_t *)sleep_array_storage, MAX_SLEEP_TASKS);

    kthread_start("kthread_task_cleaner", 0, NULL);

    is_initalized = 1;
}

static int round_robin(uint32_t tick)
{
    while (sleep_queue.size > 0)
    {
        task_t *t = sleep_queue_peek(&sleep_queue);
        if (!t || t->wake_tick > tick)
            break;

        sleep_queue_pop(&sleep_queue);
        task_queue_push(&ready_queue, t);
    }

    task_t *next;

    if (current_task() && current_task()->state == TASK_STATE_RUNNING)
    {
        task_queue_push(&ready_queue, current_task());
        set_current_task(NULL);
    }

    next = task_queue_pop(&ready_queue);
    if (!next)
    {
        dbgprintf("Out of tasks\n");
        PANIC();
        return -1;
    }

    if (next->state == TASK_STATE_CREATED)
    {
        if (next->mode != TASK_MODE_KTHREAD)
            kernel_task_state_segment_set_stack(0x10, next->kebp);

        if (current_task() == NULL || current_task()->physical_cr3 != next->physical_cr3)
            paging_manager_set_cr3(next->paging_manager, next->physical_cr3, 1);

        set_current_task(next);

        task_start_impl(next);
    }

    while (next->state != TASK_STATE_RUNNING)
    {
        switch (next->state)
        {
        case TASK_STATE_ZOMBIE:
            next->state = TASK_STATE_ZOMBIE;
            task_queue_push(&zombie_queue, next);
            break;

        case TASK_STATE_SLEEPING:
            next->state = TASK_STATE_SLEEPING;
            sleep_queue_push(&sleep_queue, next);
            break;

        case TASK_STATE_CREATED:
            if (next->mode != TASK_MODE_KTHREAD)
                kernel_task_state_segment_set_stack(0x10, next->kebp);

            if (current_task() == NULL || current_task()->physical_cr3 != next->physical_cr3)
                paging_manager_set_cr3(next->paging_manager, next->physical_cr3, 1);

            set_current_task(next);
            task_start_impl(next);
            break;

        default:
            task_queue_push(&ready_queue, next);
            break;
        }

        next = task_queue_pop(&ready_queue);
        if (!next)
        {
            dbgprintf("Out of tasks\n");
            PANIC();
            return -1;
        }
    }

    if (current_task() != next)
    {
        if (next->mode != TASK_MODE_KTHREAD)
            kernel_task_state_segment_set_stack(0x10, next->kebp);

        if (current_task() == NULL || current_task()->physical_cr3 != next->physical_cr3)
            paging_manager_set_cr3(next->paging_manager, next->physical_cr3, 1);

        set_current_task(next);
    }

    return 0;
}

static uint8_t cleaner_is_initialized = 0;
static void kthread_task_cleaner(void)
{
    dbgprintf("I'm cleaning?\n");
    if (cleaner_is_initialized)
        scheduler_exit();
    cleaner_is_initialized = 1;

    while (cleaner_is_initialized)
    {
        task_t* task = task_queue_peek(&zombie_queue);
        if(task == NULL){
            __asm__ volatile("hlt");
            continue;
        }

        task = task_queue_pop(&zombie_queue);
        if(task != NULL){
            task_cleanup(task->pid);
            scheduler_yield();
        }
    }

    scheduler_exit();
}
REGISTER_KTHREAD("kthread_task_cleaner", kthread_task_cleaner);

int scheduler_schedule(registers_t *__reg_ /*unused*/, uint32_t tick)
{
    if (__reg_ != NULL)
    {
    }

    if (current_task() == NULL)
    {
        set_current_task(task_queue_pop(&ready_queue));
    }

    {
        INTERRUPT_SAFE_BLOCK({
            _task_save_context(current_task());

            round_robin(tick);

            _task_restore_context(current_task());
        });
    }

    return 0;
}

int scheduler_add(task_t *task)
{
    if (task == NULL)
        return -1;

    task_queue_push(&ready_queue, task);

    return 0;
}

int scheduler_yield(void)
{
    return scheduler_schedule(NULL, programable_interval_timer_get_tick());
}

int scheduler_sleep(uint32_t ms)
{
    uint32_t tick_now = programable_interval_timer_get_tick();
    uint32_t hz = programable_interval_timer_get_hz();

    uint32_t ticks_to_sleep = (ms * hz) / 1000;
    if (ticks_to_sleep == 0)
        ticks_to_sleep = 1;

    current_task()->wake_tick = tick_now + ticks_to_sleep;
    current_task()->state = TASK_STATE_SLEEPING;
    sleep_queue_push(&sleep_queue, current_task());

    return scheduler_schedule(NULL, tick_now);
}

task_t *scheduler_consume(void)
{
    task_t *task = current_task();
    set_current_task(NULL);
    return task;
}

int scheduler_exit(void)
{
    if (current_task() == NULL)
        return -1;

    current_task()->state = TASK_STATE_ZOMBIE;

    INTERRUPT_SAFE_BLOCK({
        _task_save_context(current_task());

        round_robin(programable_interval_timer_get_tick());

        _task_restore_context(current_task());
    });

    return 0;
}

void _Noreturn kthread_entry(int argc, char *args[])
{
    if (current_task() == NULL)
    {
        PANIC();
    }

    void (*entry)(int argc, char *argv[]) = (void (*)(int argc, char *argv[]))current_task()->thread_eip;
    entry(argc, args);

    PANIC();
}