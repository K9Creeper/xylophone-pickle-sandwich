/// ---------------------
/// interrupt-request.hpp
/// @brief This file declares the needed functions to add handles to external (device) interrupts.

#pragma once

#include <stdint.h>

#include "../../data-structures/registers.h"

namespace Kernel
{
    namespace Interrupts
    {
        typedef void (*Interrupt_Request_Handle)(Registers *regs);

        extern void Interrupt_Request_AddHandle(uint16_t num, void* handle);
        extern void Interrupt_Request_RemoveHandle(uint16_t num);
    }
}