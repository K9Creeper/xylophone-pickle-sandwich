/// --------------------------
/// @file task-state-segment.h

#ifndef KERNEL_TASK_STATE_SEGMENT_H
#define KERNEL_TASK_STATE_SEGMENT_H

#include <stdint.h>

extern void kernel_task_state_segment_int(uint32_t idx, uint32_t kss, uint32_t kesp);
extern void kernel_task_state_segment_set_stack(uint32_t kss, uint32_t kesp);

#endif