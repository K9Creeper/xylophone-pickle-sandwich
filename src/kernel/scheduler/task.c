/// ------------
/// @file task.c

#include "task.h"

#include <kernel/util.h>
#include <memory.h>
#include <string.h>

#include <kernel/data-structures/kernel-context.h>
#include <kernel/memory-management/paging-manager.h>
#include <kernel/memory-management/heap-manager.h>

#include "scheduler.h"

#define TASKS_MAX 256

static int task_count = 0;
static task_t task_table[TASKS_MAX];

extern kernel_context_t* kernel_context;

extern void _task_start(task_t *);

extern void kthread_entry(int argc, char *args[]);

static task_t *get_free_task(void)
{
    int i;
    for (i = 0; i < TASKS_MAX; i++)
    {
        if (task_table[i].state == TASK_STATE_STOPPED)
        {
            task_t *task = &task_table[i];

            task->pid = i;
            task->state = TASK_STATE_CREATED;
            task->mode = 0;

            task->paging_manager = NULL;
            task->heap_manager = NULL;

            return task;
        }
    }

    return NULL;
}

static int init_kernel_stack(task_t *task)
{
    if ((void *)task == NULL)
        return -1;
    uint32_t stack = (uint32_t)kernel_amalloc(TASK_STACK_SIZE);

    if ((void *)stack == NULL)
        return -1;

    memset((uint8_t *)stack, 0, TASK_STACK_SIZE);

    // Stack data
    task->ctx.ebp = task->ctx.esp = (stack + TASK_STACK_SIZE) & ~0xF;
    ;
    task->kebp = task->kesp = task->ctx.esp;

    // Orginial (bottom)
    task->stackptr = stack;

    return 0;
}

void task_init(void)
{
    for (int i = 0; i < TASKS_MAX; i++)
    {
        task_table[i].state = TASK_STATE_STOPPED;
        task_table[i].pid = (uint16_t)-1;
        task_table[i].next = task_table[i].prev = task_table[i].parent = NULL;
    }
}

void _Noreturn task_start_impl(task_t *task)
{
    task->state = TASK_STATE_RUNNING;
    system_interrupt_disable_counter--;
    _task_start(task);

    PANIC();
}

int task_cleanup(uint16_t pid)
{
    if (pid == (uint16_t)-1 || pid > TASKS_MAX)
        return -1;

    task_t *task = &task_table[pid];

    for (int i = 0; i < TASKS_MAX; i++)
    {
        if (task_table[i].parent == task && task_table[i].mode == TASK_MODE_THREAD)
        {
            task_table[i].state = TASK_STATE_ZOMBIE;
        }
    }

    if (task->mode != TASK_MODE_PROCESS)
    {
        for (int i = 0; i < ALLOCATION_MAX_COUNT; i++)
        {
            if (task->thread_allocation_table.data[i].size > 0 && task->thread_allocation_table.data[i].address)
            {
                heap_manager_free(task->heap_manager, (uint32_t)task->thread_allocation_table.data[i].address);
                task->thread_allocation_table.data[i].size = 0;
                task->thread_allocation_table.data[i].address = 0;
            }
        }
    }else{
        heap_manager_free_all(task->heap_manager);
    }

    INTERRUPT_SAFE_BLOCK({
        kernel_free((void *)task->stackptr);
        task_count--;

        memset((uint8_t *)task, 0, sizeof(task_t));
        task->state = TASK_STATE_STOPPED;
        task->pid = (uint16_t)-1;
        task->next = task->prev = task->parent = NULL;
    });
}

int task_create_kthread(task_entry_routine_t entry, char *name, int argc, char **argv)
{
    DISABLE_INTERRUPTS();

    task_t *task = get_free_task();
    
    if (task == NULL)
    {
        ENABLE_INTERRUPTS();
        return -1;
    }

    if (init_kernel_stack(task))
    {
        ENABLE_INTERRUPTS();
        return -1;
    }

    task->parent = NULL;

    task->thread_eip = (uint32_t)entry;
    task->paging_manager = &kernel_context->paging_manager;
    task->heap_manager = &kernel_context->heap_manager;

    task->physical_cr3 = paging_manager_get_physical_address(&kernel_context->paging_manager, (uint32_t)kernel_context->paging_manager.page_directory);

    task->ctx.eip = (uint32_t)kthread_entry;

    task->mode = TASK_MODE_KTHREAD;

    task->cs = 0x8;
    task->ds = 0x10;

    memcpy(task->name, name, strlen(name) + 1);

    if (argc != 0)
    {
        task->argv = (char **)kernel_calloc(1, sizeof(char *) * argc);
        if (task->argv == NULL)
        {
            ENABLE_INTERRUPTS();
            return -1;
        }

        for (int i = 0; i < argc; i++)
        {
            task->argv[i] = (char *)kernel_calloc(1, 100);
            if (task->argv[i] == NULL)
            {
                ENABLE_INTERRUPTS();
                return -1;
            }

            memcpy(task->argv[i], argv[i], strlen(argv[i]) + 1);
        }

        task->args = argc;
    }

    if(scheduler_add(task)){
        // TODO: Handle this....
        //
    }

    task_count++;

    ENABLE_INTERRUPTS();

    return task->pid;
}