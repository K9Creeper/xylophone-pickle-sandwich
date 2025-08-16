/// ------
/// task.c
/// @brief This file defines the functions for init and handling tasks.

#include <scheduling/task.h>
#include <data-structures/scheduling/task.h>

#include <scheduling/scheduling.h>

#include <memory.h>
#include <string.h>

#include <kernel/stdlib.h>
#include <kernel/util.h>

#include <data-structures/kernel/kernel-context.h>

#define MAX_NUM_OF_TASKS 256

// scheduling.s
extern void _task_start(task_t *task);

// kernel-main.c
extern kernel_context_t* kernel_context;

// kthread.c
extern void kthread_entry(int argc, char *args[]);

static int task_count = 0;
static task_t task_table[MAX_NUM_OF_TASKS];

static task_t *get_free_task(void)
{
    int i;
    for (i = 0; i < MAX_NUM_OF_TASKS; i++)
    {
        if (task_table[i].state == TASK_STATE_STOPPED)
        {
            task_t *task = &task_table[i];

            task->pid = i;
            task->state = TASK_STATE_CREATED;
            task->mode = 0;
            task->args = 0;
            task->argv = NULL;

            task->paging_manger = NULL;
            task->heap_manger = NULL;
            
            task->preempts = 0;
            task->yields = 0;
            task->blocked_count = 0;

            return task;
        }
    }

    return NULL;
}


static void clone_paging_directory(page_directory_t *dst, page_directory_t *src)
{
    for (uint32_t i = 0; i < 1024; i++)
    {
        if (src->ref_tables[i] != 0)
        {
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        }
    }
}

void task_init(void){
    for (int i = 0; i < MAX_NUM_OF_TASKS; i++){
		task_table[i].state = TASK_STATE_STOPPED;
		task_table[i].pid = -1;
		task_table[i].next = NULL;
        task_table[i].is_priority = false;
	}
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
    task->ctx.ebp = task->ctx.esp = (stack + TASK_STACK_SIZE) & ~0xF;;
    task->kebp = task->kesp = task->ctx.esp;

    // Orginial (bottom)
    task->stackptr = stack;

    return 0;
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

    task->thread_eip = (uintptr_t)entry;
    task->paging_manger = &kernel_context->paging_manager;
    task->heap_manger = &kernel_context->heap_manager;

    task->physical_cr3 = paging_manager_get_physical_address(&kernel_context->paging_manager, (uint32_t)kernel_context->paging_manager.page_directory);

    task->ctx.eip = (uint32_t)kthread_entry;

    task->mode = TASK_MODE_KTHREAD;

    task->cs = 0x8;
    task->ds = 0x10;

    if(kernel_context->filesystem.mount_count > 0)
        memcpy(task->directory, "0:/", 4);

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

    scheduling_add(task);

    task_count++;

    ENABLE_INTERRUPTS();

    return task->pid;
}

void _Noreturn task_start(task_t *task)
{
    task->state = TASK_STATE_RUNNING;
    system_interrupt_disable_counter--;
    _task_start(task);

    PANIC();
}

int task_cleanup(int pid){
    if(pid < 0 || pid > MAX_NUM_OF_TASKS)
        return -1;
    
    task_t* task = &task_table[pid];

    for (int i = 0; i < MAX_NUM_OF_TASKS; i++){
		if(task_table[i].parent == task && task_table[i].mode == TASK_MODE_THREAD){
			task_table[i].state = TASK_STATE_ZOMBIE;
		}
	}

    // TODO: Cleanup virtual memory, when implemented..

    INTERRUPT_SAFE_BLOCK({
        
    kernel_free((void*)task->stackptr);
    task_count--;

    memset((uint8_t*)task, 0, sizeof(task_t));
    task->state = TASK_STATE_STOPPED;
	task->pid = -1;
	task->next = NULL;
    task->is_priority = false;

    });
    
    return 0;
}