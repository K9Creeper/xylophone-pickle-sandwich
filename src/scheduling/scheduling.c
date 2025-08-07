/// ------------
/// scheduling.c
/// @brief This file defines the core functions of system wide scheduling.

#include <scheduling/scheduling.h>
#include <data-structures/scheduling/task.h>
#include <scheduling/task.h>
#include <scheduling/task-queue.h>

#include <kernel/util.h>
#include "../kernel/drivers/pit/pit.h"

#include <stdio.h>

extern void kernel_task_state_segment_set_stack(uint32_t kss, uint32_t kesp);

void _task_restore_context();
void _task_save_context();

static uint32_t yields;
static uint32_t exits;

static task_queue_t queue;
static task_queue_t priority;

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

    task_queue_init(&priority);
    task_queue_init(&queue);

    is_initalized = true;
}

static int round_robin(uint32_t tick)
{
    task_t *next;

    // Peek at the next task
    next = task_queue_peek(&queue);
    if (next == NULL) {
        printf("Scheduler: Queue is empty\n");
        PANIC();
        return -1;
    }

    // Add current running task back to queue
    if (current_task != NULL) {
        task_queue_push(&queue, current_task);
        current_task = NULL;
    }

    // Loop until we find a runnable task
    do {
        next = task_queue_pop(&queue);
        if (next == NULL) {
            printf("Scheduler: No runnable tasks!\n");
            return -1;
        }

        switch (next->state) {
        case TASK_STATE_RUNNING:
            // Found a runnable task
            break;

        case TASK_STATE_CREATED:
            if (next->mode != TASK_MODE_KTHREAD) {
                kernel_task_state_segment_set_stack(0x10, next->kebp);
            }
            if (!(current_task && current_task->physical_cr3 == next->physical_cr3))
                paging_manager_swap_system_page_directory(next->physical_cr3, true);

            current_task = next;
            system_current_task = current_task;

            task_start(next);
            printf("Hello IS BAD\n");
            /* SHOULD NEVER RETURN */
            break;

        case TASK_STATE_ZOMBIE:
            // TODO: Cleanup resources for zombie task
            // For now, just skip it
            continue;

        case TASK_STATE_SLEEPING:
            if (next->sleep < tick) {
                next->state = TASK_STATE_RUNNING;
                break;
            }
            // fall through

        case TASK_STATE_BLOCKED:
        default:
            // Push back to queue if not runnable
            task_queue_push(&queue, next);
            continue;
        }
        // Only exit loop if we found a runnable task
    } while (next->state != TASK_STATE_RUNNING);

    // Set up context for the next task
    current_task = next;
    system_current_task = current_task;

    if (next->mode != TASK_MODE_KTHREAD) {
        kernel_task_state_segment_set_stack(0x10, next->kebp);
    }

    if (!(current_task && current_task->physical_cr3 == next->physical_cr3))
        paging_manager_swap_system_page_directory(next->physical_cr3, true);

    current_task = next;
    system_current_task = current_task;

    return 0;
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
}

int scheduling_schedule(registers_t *__reg_ /*unused*/, uint32_t tick)
{
    if (current_task == NULL)
    {
        current_task = task_queue_pop(&queue);
        system_current_task = current_task;
    }

    current_task->yields++;

    INTERRUPT_SAFE_BLOCK({
        _task_save_context(current_task);

        round_robin(tick);

        _task_restore_context(current_task);
    });

    return 0;
}

int scheduling_prioritize(task_t *task)
{
    if (task == NULL)
        return -1;

    task_queue_push(&priority, task);

    return 0;
}

int scheduling_add(task_t *task)
{
    if (task == NULL)
        return -1;

    task_queue_push(&queue, task);

    return 0;
}

int scheduling_block(task_t *task)
{
    if (task == NULL)
        return -1;

    task->state = TASK_STATE_BLOCKED;

    INTERRUPT_SAFE_BLOCK({
        _task_save_context(task);
        printf("Saved\n");
        round_robin(pit_get_tick());
        printf("Did roundy\n");
        _task_restore_context(current_task);
        printf("Restored\n");
    });

    return 0;
}

int scheduling_sleep(int time)
{
    uint32_t tick = pit_get_tick();

    current_task->sleep = tick + time;
    current_task->state = TASK_STATE_SLEEPING;

    scheduling_schedule(NULL, tick);

    return 0;
}

task_t *scheduling_consume(void)
{
    task_t *task = current_task;
    current_task = NULL;
    return task;
}