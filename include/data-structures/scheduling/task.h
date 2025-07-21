/// ------
/// task.h
/// @brief This file defines the Task structure, for keeping track of tasks/processes.

#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <registers.h>

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

typedef enum {
    TASK_STATUS_CREATED,
    TASK_STATUS_READY,
    TASK_STATUS_ZOMBIE,
    TASK_STATUS_RUNNING
} task_status_enum_t;

typedef enum {
    TASK_MODE_KERNEL,
    TASK_MODE_USER
} task_mode_enum_t;

typedef struct task_s{
    char name[512];
    registers_t state;

    task_status_enum_t status;
    task_mode_enum_t mode;
    
    uint32_t physical_cr3;

    paging_manager_t* paging_manager;
    heap_manager_t* heap_manager;

    bool is_thread;
}task_t;

#endif