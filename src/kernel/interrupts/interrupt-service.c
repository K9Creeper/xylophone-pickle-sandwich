/// -------------------
/// interrupt-service.c
/// @brief This file defines the core functions for handling and setting up
/// internal interrupts.

#include "interrupt-service.h"

#include <stddef.h>
#include <stdio.h>

static void *isrs_handles[KERNEL_INTERRUPT_SERVICE_MAX_HANNDLE_COUNT];
static void *isrs_fault_handles[KERNEL_INTERRUPT_SERVICE_FAULT_MAX_HANNDLE_COUNT];

void kernel_interrupt_service_set_handle(uint16_t idx, kernel_interrupt_service_handle_t handle)
{
    if (idx >= 0 && idx < KERNEL_INTERRUPT_SERVICE_MAX_HANNDLE_COUNT)
    {
        isrs_handles[idx] = (void *)handle;
    }
}

void kernel_interrupt_service_remove_handle(uint16_t idx)
{
    kernel_interrupt_service_set_handle(idx, NULL);
}

void kernel_interrupt_service_set_fault_handle(uint16_t idx, kernel_interrupt_service_fault_handle_t handle)
{
    if (idx >= 0 && idx < KERNEL_INTERRUPT_SERVICE_FAULT_MAX_HANNDLE_COUNT)
    {
        isrs_fault_handles[idx] = (void *)handle;
    }
}

void kernel_interrupt_service_remove_fault_handle(uint16_t idx)
{
    kernel_interrupt_service_set_fault_handle(idx, NULL);
}

// interrupt-service.s
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

// ../descriptors/interrupt-descriptor-table.c
extern void kernel_interrupt_descriptor_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

void isr_handler(registers_t r)
{
    if (r.int_no < 32)
    {
        printf("Fault %D\n", r.int_no);

        switch (r.int_no)
        {
        case 14U:
        {
            uint32_t faulting_address;

            asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

            // The error code gives us details of what happened
            uint32_t present = r.err_code & 0x1;
            uint32_t rw = r.err_code & 0x2;
            uint32_t user = r.err_code & 0x4;
            uint32_t reserved = r.err_code & 0x8;
            uint32_t inst_fetch = r.err_code & 0x10;

            printf("Possible causes: [ ");
            if (!present)
                printf("Page not present ");
            if (rw)
                printf("Page is read only ");
            if (user)
                printf("Page is read only ");
            if (reserved)
                printf("Overwrote reserved bits ");
            if (inst_fetch)
                printf("Instruction fetch ");
            printf("] at ");
            printf("0x%X\n", faulting_address);
        }
        break;
        }

        printf(
            "ds:      0x%X\n"
            "edi:     0x%X\n"
            "esi:     0x%X\n"
            "ebp:     0x%X\n"
            "esp:     0x%X\n"
            "ebx:     0x%X\n"
            "edx:     0x%X\n"
            "ecx:     0x%X\n"
            "eax:     0x%X\n"
            "int_no:  0x%X\n"
            "err_code:0x%X\n"
            "eip:     0x%X\n"
            "cs:      0x%X\n"
            "eflags:  0x%X\n"
            "useresp: 0x%X\n"
            "ss:      0x%X\n",
            r.ds, r.edi, r.esi, r.ebp, r.esp,
            r.ebx, r.edx, r.ecx, r.eax, r.int_no,
            r.err_code, r.eip, r.cs, r.eflags, r.useresp,
            r.ss);

        kernel_interrupt_service_handle_t handler = (kernel_interrupt_service_handle_t)(isrs_fault_handles[r.int_no]);
        if (handler)
        {
            handler(&r);
        }

        for (;;)
        {
            asm volatile("hlt");
        }
    }

    kernel_interrupt_service_fault_handle_t handler = (kernel_interrupt_service_fault_handle_t)(isrs_handles[r.int_no]);
    if (handler)
    {
        handler(&r);
    }
}

void kernel_interrupt_service_add_gates(void)
{
    kernel_interrupt_descriptor_set_gate(0, (uint32_t)(isr0), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(1, (uint32_t)(isr1), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(2, (uint32_t)(isr2), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(3, (uint32_t)(isr3), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(4, (uint32_t)(isr4), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(5, (uint32_t)(isr5), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(6, (uint32_t)(isr6), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(7, (uint32_t)(isr7), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(8, (uint32_t)(isr8), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(9, (uint32_t)(isr9), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(10, (uint32_t)(isr10), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(11, (uint32_t)(isr11), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(12, (uint32_t)(isr12), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(13, (uint32_t)(isr13), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(14, (uint32_t)(isr14), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(15, (uint32_t)(isr15), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(16, (uint32_t)(isr16), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(17, (uint32_t)(isr17), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(18, (uint32_t)(isr18), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(19, (uint32_t)(isr19), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(20, (uint32_t)(isr20), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(21, (uint32_t)(isr21), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(22, (uint32_t)(isr22), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(23, (uint32_t)(isr23), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(24, (uint32_t)(isr24), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(25, (uint32_t)(isr25), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(26, (uint32_t)(isr26), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(27, (uint32_t)(isr27), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(28, (uint32_t)(isr28), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(29, (uint32_t)(isr29), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(30, (uint32_t)(isr30), 0x08, 0x8E);
    kernel_interrupt_descriptor_set_gate(31, (uint32_t)(isr31), 0x08, 0x8E);
}