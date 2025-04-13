/// ---------------------
/// interrupt-service.hpp
/// @brief This file declares the functions relating to the handling of ISRS interrupts.

#pragma once

#include <stdint.h>

#include "../../data-structures/registers.h"

namespace Kernel
{
    namespace Interrupts
    {
        typedef void (*Interrupt_Service_Handle)(Registers *regs);

        extern void Interrupt_Service_AddHandle(uint16_t num, void* handle);
        extern void Interrupt_Service_RemoveHandle(uint16_t num);

        extern void Interrupt_Service_AddFaultHandle(uint16_t num, void* handle);
    }
}