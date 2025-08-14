/// --------
/// bios32.c
/// @brief This file implements the BIOS32 services.

//
// TODO: Re-write this bs

#include "bios32.h"

#include <data-structures/kernel-descriptors/global-descriptor-table.h>
#include <data-structures/kernel-descriptors/interrupt-descriptor-table.h>

#include "../descriptors/global-descriptor-table.h"
#include "../descriptors/interrupt-descriptor-table.h"

#include <memory.h>

#include <kernel/util.h>

#include <stdio.h>

// bios32.asm
extern void _bios32_helper();
extern void _bios32_helper_end();

extern void *_gdt_entries;
extern void *_gdt_ptr;
extern void *_idt_ptr;
extern void *_in_reg_ptr;
extern volatile void *_out_reg_ptr;
extern volatile void *_intnum_ptr;

#define REBASE_ADDRESS 0x7C00
#define REBASE(sym) \
    ((void *)((uint32_t)(REBASE_ADDRESS) + ((uint32_t)(sym) - (uint32_t)_bios32_helper)))

void (*rebased_bios32_helper)(void) = (void *)REBASE_ADDRESS;

interrupt_descriptor_table_ptr_t real_idt_ptr;
global_descriptor_table_ptr_t real_gdt_ptr;

static uint32_t size;

// global-descriptor-table.c
extern global_descriptor_table_entry_t pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];

extern void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

static void restore(void);

void bios32_init(void)
{
    real_gdt_ptr.base = (uint32_t)pGDT;
    real_gdt_ptr.limit = sizeof(pGDT) - 1;

    real_idt_ptr.base = 0;
    real_idt_ptr.limit = 0x3ff;

    size = (uint32_t)(_bios32_helper_end) - (uint32_t)(_bios32_helper);
}

// global-descriptor-table.s
extern void _gdt_flush(void);
// interrupt-descriptor-table.s
extern void _idt_load(void);
// ../interrupts/interrupt-request.c
extern void kernel_interrupt_request_remap(void);
void bios32_service(uint8_t interrupt_num, registers16_t *in_reg, registers16_t *out_reg)
{
    void *new_code_base = (void *)(REBASE_ADDRESS);

    memcpy((uint8_t *)(&_gdt_entries), (uint8_t *)(pGDT), sizeof(pGDT));

    real_gdt_ptr.base = (uint32_t)(REBASE((&_gdt_entries)));
    memcpy((uint8_t *)(&_gdt_ptr), (uint8_t *)(&real_gdt_ptr), sizeof(real_gdt_ptr));

    memcpy((uint8_t *)(&_idt_ptr), (uint8_t *)(&real_idt_ptr), sizeof(real_idt_ptr));

    memcpy((uint8_t *)(&_in_reg_ptr), (uint8_t *)(in_reg), sizeof(registers16_t));
    void *t = REBASE(&_in_reg_ptr);
    memcpy((uint8_t *)(&_intnum_ptr), (uint8_t *)(&interrupt_num), sizeof(uint8_t));

    memcpy((uint8_t *)(new_code_base), (uint8_t *)(_bios32_helper), size);

    DISABLE_INTERRUPTS();

    // disables interrupts
    rebased_bios32_helper();

    t = REBASE(&_out_reg_ptr);
    memcpy((uint8_t *)(out_reg), (uint8_t *)(t), sizeof(registers16_t));

    _gdt_flush();
    _idt_load();
    __asm__ __volatile__("" ::: "memory");

    kernel_interrupt_request_remap();
    __asm__ __volatile__("" ::: "memory");

    /*
        ISSUE WITH RESTORATION
        INTERRUPTS ARE ENABLED TOO QUICKLY
    */
    ENABLE_INTERRUPTS();
    __asm__ volatile("hlt" ::: "memory");
}