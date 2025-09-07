/// ---------------
/// @file kthread.c

#include "kthread.h"

#include <string.h>

#include <kernel/data-structures/kthread/kthread.h>

#include "../scheduler/task.h"

// linker.ld
extern const kthread_t __start_kthread[];
extern const kthread_t __stop_kthread[];
#include <kernel/dbgprintf.h>
int kthread_start(char* name, int argc, char* argv[]){
        
    for (const kthread_t* e = __start_kthread; e < __stop_kthread; ++e) {
        if(memcmp(name, e->name, strlen(e->name)) == 0){
            return task_create_kthread(e->entry, (char*)e->name, argc, argv);
        }
    }
    return -1;
}