/// --------------------
/// task-state-segment.c
/// @brief This file defines the functions needed to set up the Task State Segment.

#include "task-state-segment.h"

#include <kernel/data-structures/descriptors/task-state-segment.h>

#include <memory.h>

// task-state-segment.s
extern void _tss_flush();

// global-descriptor-table.c
extern void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

static task_state_segment_entry_t tss;

void kernel_task_state_segment_int(uint32_t idx, uint32_t kss, uint32_t kesp){
    uint32_t base = (uint32_t)(&tss);
    kernel_global_descriptor_set_gate(idx, base, base + sizeof(task_state_segment_entry_t) - 1, 0xE9, 0x0);
    memset((uint8_t*)(&tss), 0, sizeof(task_state_segment_entry_t));
    
    tss.ss0 = kss;
    tss.esp0 = kesp;

    tss.cs = 0x0b;
    tss.ds = 0x13;
    tss.es = 0x13;
    tss.fs = 0x13;
    tss.gs = 0x13;
    tss.ss = 0x13;

    _tss_flush();
}

void kernel_task_state_segment_set_stack(uint32_t kss, uint32_t kesp){
    tss.ss0 = kss;
    tss.esp0 = kesp;
}