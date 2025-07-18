/// -------
/// pmm.hpp
/// @brief This file contains the declarations of physical memory management init related functions.

#pragma once

#include <stdint.h>

namespace Kernel{
    namespace Memory_Management{
        extern void PMM_Init(uint32_t size);
    }
}