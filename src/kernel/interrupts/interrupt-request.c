/// -------------------
/// interrupt-request.c
/// @brief This file defines the core functions to handle external (device) interrupts,
/// and allow for the call to handles.

#include "interrupt-request.h"

#include <stddef.h>
#include <memory.h>

static void *irq_handles[KERNEL_INTERRUPT_REQUEST_MAX_HANNDLE_COUNT];

void kernel_interrupt_request_set_handle(uint16_t idx, kernel_interrupt_request_handle_t handle)
{
    if (idx >= 0 && idx < KERNEL_INTERRUPT_REQUEST_MAX_HANNDLE_COUNT)
    {
        irq_handles[idx] = (void*)handle;
    }
}

void kernel_interrupt_request_remove_handle(uint16_t idx)
{
    kernel_interrupt_request_set_handle(idx, NULL);
}

// interrupt-request.s
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// ../descriptors/interrupt-descriptor-table.c
extern void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

void irq_handler(registers_t regs)
{
    if (regs.interrupt >= 40)
    {
        outportb(0xA0, 0x20);
    }
    outportb(0x20, 0x20);

    if (regs.interrupt >= 16 && regs.interrupt - 32 < 16)
    {
        if (irq_handles[regs.interrupt - 32])
        {
            kernel_interrupt_request_handle_t handler = (kernel_interrupt_request_handle_t)(irq_handles[regs.interrupt - 32]);
            if (handler != NULL)
            {
                handler(&regs);
            }
        }
    }
}

void kernel_interrupt_request_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x00);
    outportb(0xA1, 0x00);
}

void kernel_interrupt_request_add_gates(void)
{
    kernel_interrupt_descriptor_set_gate(32, (uint32_t)(irq0), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(33, (uint32_t)(irq1), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(34, (uint32_t)(irq2), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(35, (uint32_t)(irq3), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(36, (uint32_t)(irq4), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(37, (uint32_t)(irq5), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(38, (uint32_t)(irq6), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(39, (uint32_t)(irq7), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(40, (uint32_t)(irq8), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(41, (uint32_t)(irq9), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(42, (uint32_t)(irq10), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(43, (uint32_t)(irq11), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(44, (uint32_t)(irq12), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(45, (uint32_t)(irq13), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(46, (uint32_t)(irq14), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(47, (uint32_t)(irq15), 0x08, 0x8E);
}