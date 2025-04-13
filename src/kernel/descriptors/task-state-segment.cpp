/// ----------------------
/// task-state-segment.cpp
/// @brief This file defines the functions needed to set up the Task State Segment.

#include "task-state-segment.hpp"

#include "../../data-structures/kernel-descriptors/task-state-segment.hpp"

#include "../../helpers/memory.h"

extern void Global_Descriptor_Table_SetGate(int index, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

extern "C" void _tss_flush();

static Task_State_Segment_Entry tss;

void Kernel::Descriptors::Task_State_Segment_Init(uint32_t idx, uint32_t kss, uint32_t kesp)
{
    uint32_t base = reinterpret_cast<uint32_t>(&tss);
    Global_Descriptor_Table_SetGate(idx, base, base + sizeof(Task_State_Segment_Entry) - 1, 0xE9, 0x0);
    memset(reinterpret_cast<unsigned char *>(&tss), 0, sizeof(Task_State_Segment_Entry));

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

void Kernel::Descriptors::Task_State_Segment_SetStack(uint32_t kss, uint32_t kesp)
{
    tss.ss0 = kss;
    tss.esp0 = kesp;
}