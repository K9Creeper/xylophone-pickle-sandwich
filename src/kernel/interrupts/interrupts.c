/// ------------------
/// @file interrupts.c

#include "interrupts.h"
#include "../descriptors/interrupt-descriptor-table.h"

#include <memory.h>

#define _INTERRUPT_S(x, s, f) \
    extern void ir##x(void);  \
    kernel_interrupt_descriptor_set_gate(x, (uint32_t)(ir##x), s, f)

#define _INTERRUPT(x) \
    _INTERRUPT_S(x, 0x08, 0x8E)

static interrupts_handle_t interrupts_handles[KERNEL_DESCRIPTORS_IDT_ENTRYCOUT] = {0};

void kernel_interrupts_add_service_handle(uint16_t isr_index, interrupts_handle_t handle)
{
    if (isr_index > 31 && isr_index != 128)
        return;
    interrupts_handles[isr_index] = handle;
}

void kernel_interrupts_add_request_handle(uint16_t irq_index, interrupts_handle_t handle)
{
    if (irq_index > 15)
        return;
    interrupts_handles[irq_index + 32] = handle;
}

void ir_handler(registers_t r)
{
    interrupts_handle_t handler = (interrupts_handle_t)(interrupts_handles[r.interrupt]);
    if (handler != NULL)
    {
        handler(&r);
    }

    // 32 is Handled internally
    if (r.interrupt > 32)
        IR_EOI(r.interrupt);

    if (r.interrupt < 32)
        for (;;)
        {
            asm volatile("hlt");
        }
}

void kernel_interrupts_init(void)
{
    memset((uint8_t *)interrupts_handles, 0, sizeof(interrupts_handles));
    // ISRs
    _INTERRUPT(0);
    _INTERRUPT(1);
    _INTERRUPT(2);
    _INTERRUPT(3);
    _INTERRUPT(4);
    _INTERRUPT(5);
    _INTERRUPT(6);
    _INTERRUPT(7);
    _INTERRUPT(8);
    _INTERRUPT(9);
    _INTERRUPT(10);
    _INTERRUPT(11);
    _INTERRUPT(12);
    _INTERRUPT(13);
    _INTERRUPT(14);
    _INTERRUPT(15);
    _INTERRUPT(16);
    _INTERRUPT(17);
    _INTERRUPT(18);
    _INTERRUPT(19);
    _INTERRUPT(20);
    _INTERRUPT(21);
    _INTERRUPT(22);
    _INTERRUPT(23);
    _INTERRUPT(24);
    _INTERRUPT(25);
    _INTERRUPT(26);
    _INTERRUPT(27);
    _INTERRUPT(28);
    _INTERRUPT(29);
    _INTERRUPT(30);
    _INTERRUPT(31);

    // SYSCALLS
    _INTERRUPT_S(128, 0x08, 0xEE);

    // IRQs
    _INTERRUPT(32);
    _INTERRUPT(33);
    _INTERRUPT(34);
    _INTERRUPT(35);
    _INTERRUPT(36);
    _INTERRUPT(37);
    _INTERRUPT(38);
    _INTERRUPT(39);
    _INTERRUPT(40);
    _INTERRUPT(41);
    _INTERRUPT(42);
    _INTERRUPT(43);
    _INTERRUPT(44);
    _INTERRUPT(45);
    _INTERRUPT(46);
    _INTERRUPT(47);
}