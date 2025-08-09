/// ---------
/// kthread.c
/// @brief This file defines the functions for handling kernel threads.

#include "kthread.h"

#include <kernel/util.h>

#include <scheduling/task.h>

#include <memory.h>
#include <string.h>
#include <stdio.h>

#define MAX_KTHREADS 64

// scheduling.c
extern task_t* system_current_task;

static int total_kthreads = 0;
static kthread_t kthread_table[MAX_KTHREADS];

int kthread_start(char* name, int argc, char* argv[]){
    for (int i = 0; i < total_kthreads; i++){
        if(memcmp(name, kthread_table[i].name, strlen(kthread_table[i].name)) == 0){
            return task_create_kthread(kthread_table[i].entry, kthread_table[i].name, argc, argv);
        }
    }
}

int kthread_register(kthread_entry_t entry, char* name){
    if((void*)name == NULL || (void*)entry == NULL) return -1;

    if(strlen(name)+1 > TASK_MAX_NAME_LENGTH || total_kthreads == MAX_KTHREADS)
        return -1;

    kthread_table[total_kthreads].entry = entry;
    memcpy((uint8_t*)kthread_table[total_kthreads].name, name, strlen(name)+1);
    total_kthreads++;

    return 0;
}

void _Noreturn kthread_entry(int argc, char* args[])
{   
    if(!system_current_task)
    {
        // Ya, this is REALLY BAD
    }

    void (*entry)(int argc, char* argv[]) = (void (*)(int argc, char* argv[]))system_current_task->thread_eip;
    entry(argc, args);

    PANIC();
}
