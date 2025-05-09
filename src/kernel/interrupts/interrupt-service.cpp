/// ---------------------
/// interrupt-service.cpp
/// @brief This file defines the core functions for handling and setting up
/// internal interrupts.

#include "interrupt-service.hpp"

extern "C"
{
    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
    void isr10();
    void isr11();
    void isr12();
    void isr13();
    void isr14();
    void isr15();
    void isr16();
    void isr17();
    void isr18();
    void isr19();
    void isr20();
    void isr21();
    void isr22();
    void isr23();
    void isr24();
    void isr25();
    void isr26();
    void isr27();
    void isr28();
    void isr29();
    void isr30();
    void isr31();
}

extern void Interrupt_Descriptor_SetGate(unsigned char num, unsigned int base, unsigned short sel, unsigned char flags);

static void *isrs_handles[32];
static void *isrs_fault_handles[32];

extern "C" void isr_handler(Registers r)
{
    if (r.int_no < 32)
    {
        Kernel::Interrupts::Interrupt_Service_Handle handler = reinterpret_cast<Kernel::Interrupts::Interrupt_Service_Handle>(isrs_fault_handles[r.int_no]);
        if (handler)
        {
            handler(&r);
        }
    }

    Kernel::Interrupts::Interrupt_Service_Handle handler = reinterpret_cast<Kernel::Interrupts::Interrupt_Service_Handle>(isrs_handles[r.int_no]);
    if (handler)
    {
        handler(&r);
    }
}

void Interrupt_Service_AddGates()
{
    Interrupt_Descriptor_SetGate(0, reinterpret_cast<uint32_t>(isr0), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(1, reinterpret_cast<uint32_t>(isr1), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(2, reinterpret_cast<uint32_t>(isr2), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(3, reinterpret_cast<uint32_t>(isr3), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(4, reinterpret_cast<uint32_t>(isr4), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(5, reinterpret_cast<uint32_t>(isr5), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(6, reinterpret_cast<uint32_t>(isr6), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(7, reinterpret_cast<uint32_t>(isr7), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(8, reinterpret_cast<uint32_t>(isr8), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(9, reinterpret_cast<uint32_t>(isr9), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(10, reinterpret_cast<uint32_t>(isr10), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(11, reinterpret_cast<uint32_t>(isr11), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(12, reinterpret_cast<uint32_t>(isr12), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(13, reinterpret_cast<uint32_t>(isr13), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(14, reinterpret_cast<uint32_t>(isr14), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(15, reinterpret_cast<uint32_t>(isr15), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(16, reinterpret_cast<uint32_t>(isr16), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(17, reinterpret_cast<uint32_t>(isr17), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(18, reinterpret_cast<uint32_t>(isr18), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(19, reinterpret_cast<uint32_t>(isr19), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(20, reinterpret_cast<uint32_t>(isr20), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(21, reinterpret_cast<uint32_t>(isr21), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(22, reinterpret_cast<uint32_t>(isr22), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(23, reinterpret_cast<uint32_t>(isr23), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(24, reinterpret_cast<uint32_t>(isr24), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(25, reinterpret_cast<uint32_t>(isr25), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(26, reinterpret_cast<uint32_t>(isr26), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(27, reinterpret_cast<uint32_t>(isr27), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(28, reinterpret_cast<uint32_t>(isr28), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(29, reinterpret_cast<uint32_t>(isr29), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(30, reinterpret_cast<uint32_t>(isr30), 0x08, 0x8E);
    Interrupt_Descriptor_SetGate(31, reinterpret_cast<uint32_t>(isr31), 0x08, 0x8E);
}

namespace Kernel
{
    namespace Interrupts
    {
        void Interrupt_Service_AddHandle(uint16_t num, void *handle)
        {
            isrs_handles[num] = handle;
        }

        void Interrupt_Service_RemoveHandle(uint16_t num)
        {
            isrs_handles[num] = nullptr;
        }

        void Interrupt_Service_AddFaultHandle(uint16_t num, void *handle)
        {
            isrs_fault_handles[num] = handle;
        }
    }
}