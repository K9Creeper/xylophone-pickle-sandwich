/// --------------
/// task-cleaner.c
/// @brief This file defines the functions and thread to cleaning ZOMBIE or exited tasks.

#include <scheduling/task-cleaner.h>
#include <scheduling/task-queue.h>
#include <scheduling/task.h>

#include <spinlock.h>
#include <syscall.h>

#include <stdio.h>

static task_queue_t to_clean_queue;
static bool is_initialized = false;

int task_cleaner_add(task_t* task_to_clean){
    if(!is_initialized || !task_to_clean){ return -1; }
    task_to_clean->state = TASK_STATE_CLEANING;
    int ret = 0;
    if(ret = task_queue_push(&to_clean_queue, task_to_clean)){
        task_to_clean->state = TASK_STATE_ZOMBIE;
        return ret;
    }
    return 0;
}

extern void scheduling_maintenance(void);

void kthread_task_cleaner(void){
    if(is_initialized) exit();
    task_queue_init(&to_clean_queue);
    is_initialized = true;

    while(is_initialized){
        scheduling_maintenance();

        task_t* task = task_queue_peek(&to_clean_queue);
        if(task == NULL){
            __asm__ volatile("hlt");
            continue;
        }

        task = task_queue_pop(&to_clean_queue);
        if(task != NULL){
            task_cleanup(task->pid);
            yield();
        }
    }
}