/// ------
/// task.h
/// @brief This file defines the task structures, for holding task/process data.

#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <memory-management/paging-manager.h>
#include <memory-management/heap-manager.h>

#define TASK_MAX_NAME_LENGTH 25
#define TASK_STACK_SIZE 0x2000

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
} task_context_t;

typedef struct task_s
{
    task_context_t ctx;   // 0 (size is 148)
    int args;             // 148
    char **argv;          // 152
    uint32_t ds;          // 156
    uint32_t cs;          // 160
    uint32_t kesp;        // 164
    uint32_t kebp;        // 168
    uintptr_t thread_eip; // 172

    char name[TASK_MAX_NAME_LENGTH];
    volatile task_state_t state;
    task_mode_t mode;
    int16_t pid;
    uint32_t sleep;
    uint32_t stackptr;
    paging_manager_t *paging_manger;
    heap_manager_t *heap_manger;
    uint32_t physical_cr3;
    int preempts;
    int yields;
    uint32_t blocked_count;

    bool is_priority;

    struct task_s *parent;
    struct task_s *next;
    struct task_s *prev;
} __attribute__((__packed__)) task_t;

typedef struct task_info_s
{
    int16_t pid;
    task_state_t state;
    uint32_t stack;
    char name[TASK_MAX_NAME_LENGTH];
} task_info_t;

typedef void (*task_entry_routine_t)(void);

#endif