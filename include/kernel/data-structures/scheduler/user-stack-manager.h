/// --------------------------
/// @file user-stack-manager.h

#ifndef USER_STACK_MANAGER_H
#define USER_STACK_MANAGER_H

#include <stdint.h>

#include <kernel/data-structures/scheduler/task.h>

#include <spinlock.h>

#define USER_STACKS_MAX (USERSPACE_TASK_STACK_TOP - USERSPACE_TASK_STACK_BOTTOM) / TASK_STACK_SIZE

typedef struct user_stack_s {
    uint32_t base;
    uint32_t top;    
    uint8_t in_use;
} user_stack_t;

typedef struct user_stack_manager_s {
    spinlock_t lock;
    user_stack_t stacks[USER_STACKS_MAX];
} user_stack_manager_t;

#endif