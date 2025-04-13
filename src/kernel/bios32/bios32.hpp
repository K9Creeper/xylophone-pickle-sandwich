/// ----------
/// bios32.hpp
/// @brief This file declares the functions to use bios calls from x86 protected mode.
#pragma once

#include "../../data-structures/registers.h"

namespace Kernel{
    namespace Bios32{
        extern void Init();
        extern void Call(uint8_t n, Registers16 * in_reg, Registers16 * out_reg, bool bEnableInt = false);
    }
}