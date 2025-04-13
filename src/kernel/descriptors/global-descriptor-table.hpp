/// ---------------------------
/// global-descriptor-table.hpp
/// @brief This file declares the functions for setting up the Global Descriptor Table within the kernel.

#pragma once

#define KERNEL_DESCRIPTORS_GDT_ENTRYCOUT 8

namespace Kernel{
    namespace Descriptors{
        extern void Global_Descriptor_Table_Init();
        extern void Global_Descriptor_Table_Install();
        
    }
}