/// -------------
/// scheduling.h
/// @brief This file declares the functions that handle system scheduling.

#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <data-structures/scheduling/task.h>
#include <registers.h>

extern void scheduling_init(void);
extern int scheduling_exit(void);

extern int scheduling_prioritize(task_t* task);
extern int scheduling_add(task_t* task);
extern int scheduling_block(task_t* task);
extern int scheduling_sleep(int ms);
extern task_t* scheduling_consume(void);
extern int scheduling_yield(void);

extern task_t* scheduling_get_task_by_pid(uint16_t id);
extern task_t* scheduling_get_task_by_name(const char* name);

extern int scheduling_schedule(registers_t*, uint32_t);

#endif