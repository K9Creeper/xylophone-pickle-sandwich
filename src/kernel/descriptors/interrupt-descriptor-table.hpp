/// ------------------------------
/// interrupt-descriptor-table.hpp
/// @brief This file declares the needed function to setup the Interrupt Descriptor Table.

#pragma once

namespace Kernel{
    namespace Descriptors{
        extern void Interrupt_Descriptor_Table_Init();
        extern void Interrupt_Descriptor_Table_Install();
    }
}