/// --------------------------
/// @file user-stack-manager.c

#include <kernel/util.h>

#include <kernel/scheduler/user-stack-manager.h>

void user_stack_manager_init(user_stack_manager_t *manager)
{
    if (!manager)
        return;
    INTERRUPT_SAFE_BLOCK({
        spinlock_init(&manager->lock);
        spinlock_lock(&manager->lock);
        for (int i = 0; i < USER_STACKS_MAX; i++)
        {
            manager->stacks[i].in_use = 0;
            manager->stacks[i].top = USERSPACE_TASK_STACK_TOP - i * TASK_STACK_SIZE;
            manager->stacks[i].base = manager->stacks[i].top - TASK_STACK_SIZE;
        }
        spinlock_unlock(&manager->lock);
    });
}

int user_stack_alloc(user_stack_manager_t *manager, uint32_t *stack_base, uint32_t *stack_top) {
    if (!manager || !stack_base || !stack_top)
        return -1;
    DISABLE_INTERRUPTS();
        spinlock_lock(&manager->lock);
        for (int i = 0; i < USER_STACKS_MAX; i++) {
            if (!manager->stacks[i].in_use) {
                manager->stacks[i].in_use = 1;
                *stack_base = manager->stacks[i].base;
                *stack_top  = manager->stacks[i].top;
                spinlock_unlock(&manager->lock);
    ENABLE_INTERRUPTS();
                return i;
            }
        }
        spinlock_unlock(&manager->lock);
    ENABLE_INTERRUPTS();
    return -1;
}

void user_stack_free(user_stack_manager_t *manager, int index) {
    if (!manager || index < 0 || index >= USER_STACKS_MAX)
        return;
    INTERRUPT_SAFE_BLOCK({
        spinlock_lock(&manager->lock);
        manager->stacks[index].in_use = 0;
        spinlock_unlock(&manager->lock);
    });
}