/// ------------------------------
/// interrupt-descriptor-table.cpp
/// @brief This file defines core functions to setup the Interrupt Descriptor Table, 
/// aswell as setting up all interrupt gates.

#include "interrupt-descriptor-table.hpp"

#include <stdint.h>

#include "../../data-structures/kernel-descriptors/interrupt-descriptor-table.hpp"

#include "../../helpers/memory.h"

Interrupt_Descriptor_Table_Entry pIDT[256];
Interrupt_Descriptor_Table_Pointer _pIDT;

extern "C" void _idt_load();

void Interrupt_Descriptor_SetGate(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags)
{
    pIDT[num].base_high = (base >> 16) & 0xFFFF;
    pIDT[num].base_low = (base & 0xFFFF);
    pIDT[num].always0 = 0;
    pIDT[num].selector = selector;
    pIDT[num].flags = flags;
}

void Kernel::Descriptors::Interrupt_Descriptor_Table_Init(){
    _pIDT.limit = (sizeof(Interrupt_Descriptor_Table_Entry) * 256) - 1;
    _pIDT.base = (unsigned int)(&pIDT);
    memset((unsigned char *)(&pIDT), 0, sizeof(Interrupt_Descriptor_Table_Entry) * 256);
}

extern void Interrupt_Request_Remap();
extern void Interrupt_Request_AddGates();

extern void Interrupt_Service_AddGates();

void Kernel::Descriptors::Interrupt_Descriptor_Table_Install(){
    Interrupt_Request_Remap();
    Interrupt_Service_AddGates();
    Interrupt_Request_AddGates();

    _idt_load();
}