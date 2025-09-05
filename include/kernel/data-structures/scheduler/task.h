/// ------------
/// @file task.h

#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#include <kernel/data-structures/memory-management/paging-manager.h>
#include <kernel/data-structures/memory-management/heap-manager.h>

typedef enum task_state_e
{
    TASK_STATE_STOPPED,
    TASK_STATE_RUNNING,
    TASK_STATE_CREATED,
    TASK_STATE_BLOCKED,
    TASK_STATE_SLEEPING,
    TASK_STATE_ZOMBIE,
    TASK_STATE_CLEANING
} task_state_t;

typedef enum task_mode_e
{
    TASK_MODE_KTHREAD,
    TASK_MODE_PROCESS,
    TASK_MODE_THREAD
} task_mode_t;

typedef struct task_context_s
{
    uint32_t eax;           // 0
    uint32_t ecx;           // 4
    uint32_t edx;           // 8
    uint32_t ebx;           // 12
    uint32_t esp;           // 16
    uint32_t ebp;           // 20
    uint32_t esi;           // 24
    uint32_t edi;           // 28
    uint32_t eip;           // 32
    uint32_t eflags;        // 36
    uint8_t fpu_state[108]; // 40 -> 148
} __attribute__((__packed__)) task_context_t;

typedef struct task_s
{
    task_context_t ctx;   // 0 (size is 148)
    uint32_t ds;          // 148
    uint32_t cs;          // 152
    uint32_t kesp;        // 156
    uint32_t kebp;        // 160
    uint32_t thread_eip; // 164

    task_state_t state;
    task_mode_t mode;
    
    uint32_t stackptr;
    uint32_t physical_cr3;
    paging_manager_t *paging_manger;
    heap_manager_t *heap_manger;

    uint32_t wakeup_tick;

} __attribute__((__packed__)) task_t;

#endif