/// --------------------
/// task-state-segment.h
/// @brief This file declares the functions needed to set up the Task State Segment.

#ifndef KERNEL_DESCRIPTOR_TASK_STATE_SEGMENT_H
#define KERNEL_DESCRIPTOR_TASK_STATE_SEGMENT_H

#include <stdint.h>

extern void kernel_task_state_segment_int(uint32_t idx, uint32_t kss, uint32_t kesp);
extern void kernel_task_state_segment_set_stack(uint32_t kss, uint32_t kesp);

#endif