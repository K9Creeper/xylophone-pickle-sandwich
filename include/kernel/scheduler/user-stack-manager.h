/// --------------------------
/// @file user-stack-manager.h

#ifndef _USER_STACK_MANAGER_H
#define _USER_STACK_MANAGER_H

#include <kernel/data-structures/scheduler/user-stack-manager.h>

extern void user_stack_manager_init(user_stack_manager_t* manager);

extern int user_stack_alloc(user_stack_manager_t *manager, uint32_t *stack_base, uint32_t *stack_top);
extern void user_stack_free(user_stack_manager_t *manager, int index);

#endif