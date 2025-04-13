/// ---------------------------
/// global-descriptor-table.cpp
/// @brief This file defines the functions for setting up the Global Descriptor Table within the kernel.

#include "global-descriptor-table.hpp"

#include <stdint.h>

#include "../../data-structures/kernel-descriptors/global-descriptor-table.hpp"

Global_Descriptor_Table_Entry pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];
Global_Descriptor_Table_Pointer _pGDT;

extern "C" void _gdt_flush();

void Global_Descriptor_Table_SetGate(int index, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
    pGDT[index].base_low = (base & 0xFFFF);
    pGDT[index].base_middle = (base >> 16) & 0xFF;
    pGDT[index].base_high = (base >> 24 & 0xFF);

    pGDT[index].limit_low = (limit & 0xFFFF);
    pGDT[index].granularity = ((limit >> 16) & 0x0F);

    pGDT[index].granularity |= (gran & 0xF0);
    pGDT[index].access = access;
}

void Kernel::Descriptors::Global_Descriptor_Table_Init(){
    _pGDT.limit = (sizeof(Global_Descriptor_Table_Entry) * KERNEL_DESCRIPTORS_GDT_ENTRYCOUT) - 1;
    _pGDT.base = (uint32_t)(&pGDT);
}

void Kernel::Descriptors::Global_Descriptor_Table_Install(){
    Global_Descriptor_Table_SetGate(0, 0, 0, 0, 0);

    Global_Descriptor_Table_SetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    Global_Descriptor_Table_SetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    Global_Descriptor_Table_SetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); 
    Global_Descriptor_Table_SetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    _gdt_flush();
}