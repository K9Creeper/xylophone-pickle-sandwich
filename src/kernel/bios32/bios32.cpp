/// ----------
/// bios32.cpp
/// @brief This file defines the functions to access and call BIOS functions.

#include "bios32.hpp"

#include "../../helpers/memory.h"

#include "../../data-structures/kernel-descriptors/global-descriptor-table.hpp"
#include "../../data-structures/kernel-descriptors/interrupt-descriptor-table.hpp"

#include "../descriptors/global-descriptor-table.hpp"
#include "../descriptors/interrupt-descriptor-table.hpp"

#define REBASED_OFFSET 0x7C00

void (*Rebased_Bios32_Helper)() = reinterpret_cast<void (*)()>(REBASED_OFFSET);

extern "C" void Bios32_Helper();
extern "C" void Bios32_HelperEnd();

#define REBASE(x) reinterpret_cast<void *>(REBASED_OFFSET + reinterpret_cast<uint32_t>(x) - reinterpret_cast<uint32_t>(Bios32_Helper))

extern "C" void *asm_gdt_entries;
extern "C" void *asm_gdt_ptr;
extern "C" void *asm_idt_ptr;
extern "C" void *asm_in_reg_ptr;
extern "C" void *asm_out_reg_ptr;
extern "C" void *asm_intnum_ptr;

Interrupt_Descriptor_Table_Pointer rIDTptr;
Global_Descriptor_Table_Pointer rGDTptr;

extern Global_Descriptor_Table_Entry* pGDT;

extern void Global_Descriptor_Table_SetGate(int index, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

void Kernel::Bios32::Init()
{
    Global_Descriptor_Table_SetGate(6, 0, 0xFFFFFFFF, 0x9A, 0x0F);

    Global_Descriptor_Table_SetGate(7, 0, 0xFFFFFFFF, 0x92, 0x0F);

    rGDTptr.base = reinterpret_cast<uint32_t>(pGDT);
    rGDTptr.limit = sizeof(pGDT) - 1;

    rIDTptr.base = 0;
    rIDTptr.limit = 0x3ff;
}

static void Restore(){
    Kernel::Descriptors::Global_Descriptor_Table_Init();

    Kernel::Bios32::Init();

    Kernel::Descriptors::Global_Descriptor_Table_Install();
    
    Kernel::Descriptors::Interrupt_Descriptor_Table_Init();

    Kernel::Descriptors::Interrupt_Descriptor_Table_Install();
}

void Kernel::Bios32::Call(uint8_t int_num, Registers16 *in_reg, Registers16 *out_reg, bool bEnableInt)
{
    void *new_code_base = reinterpret_cast<void *>(REBASED_OFFSET);

    memcpy(reinterpret_cast<unsigned char *>(&asm_gdt_entries), reinterpret_cast<unsigned char *>(pGDT), sizeof(pGDT));

    rGDTptr.base = reinterpret_cast<uint32_t>(REBASE((&asm_gdt_entries)));
    memcpy(reinterpret_cast<unsigned char *>(&asm_gdt_ptr), reinterpret_cast<unsigned char *>(&rGDTptr), sizeof(rGDTptr));

    memcpy(reinterpret_cast<unsigned char *>(&asm_idt_ptr), reinterpret_cast<unsigned char *>(&rIDTptr), sizeof(rIDTptr));

    memcpy(reinterpret_cast<unsigned char *>(&asm_in_reg_ptr), reinterpret_cast<unsigned char *>(in_reg), sizeof(Registers16));
    void *t = REBASE(&asm_in_reg_ptr);
    memcpy(reinterpret_cast<unsigned char *>(&asm_intnum_ptr), reinterpret_cast<unsigned char *>(&int_num), sizeof(uint8_t));

    uint32_t size = reinterpret_cast<uint32_t>(Bios32_HelperEnd) - reinterpret_cast<uint32_t>(Bios32_Helper);
    memcpy(reinterpret_cast<unsigned char *>(new_code_base), reinterpret_cast<unsigned char *>(Bios32_Helper), size);

    Rebased_Bios32_Helper();

    t = REBASE(&asm_out_reg_ptr);
    memcpy(reinterpret_cast<unsigned char *>(out_reg), reinterpret_cast<unsigned char *>(t), sizeof(Registers16));

    asm volatile("cli");

    Restore();

    if(bEnableInt)
        asm volatile("sti");
}