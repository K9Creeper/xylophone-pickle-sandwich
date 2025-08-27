/// ------------------
/// @file interrupts.h

#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

#include <stdint.h>
#include <kernel/data-structures/interrupts/interrupts.h>

extern void kernel_interrupts_init(void);

extern void kernel_interrupts_add_service_handle(uint16_t isr_index, interrupts_handle_t handle);
extern void kernel_interrupts_add_request_handle(uint16_t irq_index, interrupts_handle_t handle);

#include <kernel/drivers/pic-8259.h>

#define IR_EOI(idx) do{ if ((idx) >= PIC_SLAVE_OFFSET) outportb(PIC2, 0x20); outportb(PIC1, 0x20); }while(0)

#endif