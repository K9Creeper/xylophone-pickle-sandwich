/// ------------
/// scheduling.c
/// @brief This file defines the functions that handle task scheduling.

#include <scheduling/scheduling.h>

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <ordered-array.h>

#include <stdio.h>

#include "../kernel/drivers/pit/pit.h"

bool scheduling_is_initialized;

static ordered_array_t task_list;

static task_t *prev_task;
static task_t *curr_task;
static task_t *to_delete_task;

// scheduling.asm
extern void _context_switch_user(registers_t *state);
extern void _context_switch_kernel(registers_t *state);

// kheap.c
extern heap_manager_t kernel_heap_manager;
// paging.c
extern paging_manager_t kernel_paging_manager;

// heap-manager.c
extern heap_manager_t *current_system_heap_manager;
// paging-manager.c
extern page_directory_t *current_system_page_directory;
extern paging_manager_t *current_system_paging_manager;

static bool is_priority_task(const type_t a, const type_t b)
{
    return false;
}

static void switch_to_task(task_t *next)
{
    curr_task = next;
    if (!(curr_task->paging_manager && curr_task->heap_manager && curr_task->paging_manager->page_directory))
        return;

    paging_manager_swap_system_page_directory(curr_task->paging_manager, curr_task->physical_cr3, true);

    if (curr_task->status == TASK_STATUS_CREATED && curr_task->mode == TASK_MODE_USER)
    {
        if (!curr_task->is_thread)
        {
            heap_manager_pre_init(curr_task->heap_manager, 0x40000000);
            printf("Pre Heap\n");
        }

        curr_task->paging_manager->heap_manager = curr_task->heap_manager;
        printf("Correct Heap\n");

        if (!curr_task->is_thread)
        {
            heap_manager_init(curr_task->heap_manager, 0x40400000, 0x40500000, 0x4FFFFF00,
                              false, false, false, curr_task->paging_manager);
            printf("Finish Heap\n");
        }

        paging_manager_allocate_range(curr_task->paging_manager, 0x70000000, 0x70500000, false, true);
        printf("Stack it\n");
    }
    else if (curr_task->status == TASK_STATUS_CREATED)
    {
        if (!curr_task->is_thread)
        {
            heap_manager_pre_init(curr_task->heap_manager, 0xD1000000);
        }

        curr_task->paging_manager->heap_manager = curr_task->heap_manager;

        if (!curr_task->is_thread)
        {
            heap_manager_init(curr_task->heap_manager, 0xD1400000, 0xD1500000, 0xDFFFFF00, true, false, false, curr_task->paging_manager);
        }

        paging_manager_allocate_range(curr_task->paging_manager, 0xD0000000, 0xD0040000, true, true);
    }

    curr_task->status = TASK_STATUS_RUNNING;

    if (curr_task->mode == TASK_MODE_USER)
    {
        _context_switch_user(&curr_task->state);
        prev_task = curr_task;
    }
    else
    {
        _context_switch_kernel(&curr_task->state); 
        prev_task = curr_task;
    }
    
}

int IRQ_disable_counter = 0;

static void lock_scheduler(void) {
    __asm__ volatile("cli");
    IRQ_disable_counter++;
}

static void unlock_scheduler(void) {
    IRQ_disable_counter--;
    if(IRQ_disable_counter == 0) {
        __asm__ volatile("sti");
    }
}

// pit called
static void scheduler(registers_t *regs, uint32_t tick)
{
    lock_scheduler();
    if (to_delete_task)
    {
        if (prev_task == to_delete_task)
            prev_task = NULL;

        // Free..

        to_delete_task = NULL;
    }

    if (task_list.size == 0 || !regs)
        return;

    if (prev_task && prev_task->status != TASK_STATUS_ZOMBIE)
    {
        prev_task->status = TASK_STATUS_READY;
        memcpy((uint8_t *)(&prev_task->state), (uint8_t *)regs, sizeof(registers_t));
        if (prev_task->mode == TASK_MODE_USER)
            prev_task->state.esp = prev_task->state.useresp;
    }

    static uint32_t prevIdx = 0;
    task_t *next = NULL;

    if (prevIdx >= task_list.size - 1)
        prevIdx = 0;

    for (uint32_t i = prevIdx; i < task_list.size; i++)
    {
        if (prevIdx >= task_list.size - 1)
        {
            prevIdx = 0;
        }

        task_t *t = (task_t *)ordered_array_get(&task_list, i);
        if (!t || t->status == TASK_STATUS_ZOMBIE)
        {
            ordered_array_remove(&task_list, i);
            if (i - 1 >= 0)
                i--;

            continue;
        }

        if (t == curr_task && task_list.size > 1)
        {
            continue;
        }

        next = t;
        prevIdx = i;

        break;
    }

    if (next && next != curr_task)
    {
        // printf("Switching to %s\n", next->name);
        switch_to_task(next);
    }
    unlock_scheduler();
}

void scheduling_init(void)
{
    if (scheduling_is_initialized)
        return;

    ordered_array_init(&task_list, MAX_TASK_COUNT, is_priority_task);
    ordered_array_place(&task_list, malloc(sizeof(task_t) * MAX_TASK_COUNT));

    scheduling_is_initialized = true;
}

void scheduling_install(void)
{
    if (!scheduling_is_initialized)
        return;

    pit_add_handle(scheduler);
}

void scheduling_add_task(task_t *task)
{
    if (!scheduling_is_initialized)
        return;

    ordered_array_insert(&task_list, (type_t)task);
}

void scheduling_exit_task(void)
{
    if (!curr_task)
        return;

    curr_task->status = TASK_STATUS_ZOMBIE;

    for (;;);
}

static void clone_paging_directory(page_directory_t *dst, page_directory_t *src)
{
    for (uint32_t i = 0; i < 1024; i++)
    {
        if (kernel_paging_manager.page_directory->ref_tables[i] == src->ref_tables[i])
        {
            dst->tables[i] = src->tables[i];
            dst->ref_tables[i] = src->ref_tables[i];
        }
    }
}

void scheduling_create_task(const char *name, void *routine, bool is_kernel, bool is_thread)
{
    bool is_creator_kernel = !curr_task || (curr_task && curr_task->mode == TASK_MODE_KERNEL);

    if (!is_creator_kernel && is_kernel)
        return;

    task_t *t = (task_t *)(malloc(sizeof(task_t)));
    memset((uint8_t *)(t->name), 0, 512);
    memcpy((uint8_t *)(t->name), (const uint8_t *)(name), strlen(name));

    t->status = TASK_STATUS_CREATED;
    t->state.eip = (uint32_t)routine; // @note: this must be correctly mapped by the pager..
    t->state.eflags = 0x206;

    if (is_kernel)
        t->mode = TASK_MODE_KERNEL;
    else
        t->mode = TASK_MODE_USER;

    t->is_thread = is_thread;

    t->paging_manager = (paging_manager_t *)amalloc(sizeof(paging_manager_t));

    if (!is_thread)
    {
        t->heap_manager = (heap_manager_t *)amalloc(sizeof(heap_manager_t));
    }
    else
    {
        t->heap_manager = current_system_heap_manager;
    }

    paging_manager_init(t->paging_manager, (page_directory_t *)amalloc(sizeof(page_directory_t)), current_system_heap_manager, true);

    t->physical_cr3 = paging_manager_get_physical_address(current_system_paging_manager, (uint32_t)t->paging_manager->page_directory);

    // So, there is an issue here for USERMODE....it can't access this.
    // I should do something, about this...limit access to specific parts in the kernel space.
    // ^^ this isn't the right way to do things, but....i don't really care rn
    clone_paging_directory(t->paging_manager->page_directory, current_system_paging_manager->page_directory);

    if (!is_kernel)
    {
        paging_manager_allocate_range(t->paging_manager, 0x40000000, 0x40400000, false, true);
    }
    else
    {
        paging_manager_allocate_range(t->paging_manager, 0xD1000000, 0xD1400000, true, true);
    }

    if (!is_kernel)
        t->state.esp = t->state.ebp = 0x70500000;
    else
    {
        t->state.esp = t->state.ebp = 0xD0040000;
    }

    scheduling_add_task(t);
}