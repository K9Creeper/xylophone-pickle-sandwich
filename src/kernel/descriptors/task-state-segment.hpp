/// ----------------------
/// task-state-segment.hpp
/// @brief This file declares the functions needed to set up the Task State Segment.

#pragma once

#include <stdint.h>

namespace Kernel{
    namespace Descriptors{
        extern void Task_State_Segment_Init(uint32_t idx, uint32_t kss, uint32_t kesp);
        extern void Task_State_Segment_SetStack(uint32_t kss, uint32_t kesp);
    }
}