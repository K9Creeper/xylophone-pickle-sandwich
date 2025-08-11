/// --------
/// bios32.c
/// @brief This file implements the BIOS32 services.

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
extern void *_out_reg_ptr;
extern void *_intnum_ptr;

#define REBASE_ADDRESS 0x7C00
#define REBASE(sym) \
    ((void *)((uintptr_t)(REBASE_ADDRESS) + ((uintptr_t)(sym) - (uintptr_t)_bios32_helper)))

void (*rebased_bios32_helper)() = (void *)REBASE_ADDRESS;

interrupt_descriptor_table_ptr_t real_idt_ptr;
global_descriptor_table_ptr_t real_gdt_ptr;

// global-descriptor-table.c
extern global_descriptor_table_entry_t pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];

extern void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#define GDT_ENTRY_SIZE (sizeof(global_descriptor_table_entry_t) * KERNEL_DESCRIPTORS_GDT_ENTRYCOUT)

static void restore(void);

void bios32_init(void)
{
    // 16 BIT
    kernel_global_descriptor_set_gate(6, 0, 0xFFFFFFFF, 0x9A, 0x0F);
    kernel_global_descriptor_set_gate(7, 0, 0xFFFFFFFF, 0x92, 0x0F);

    real_gdt_ptr.base = (uint32_t)pGDT;
    real_gdt_ptr.limit = GDT_ENTRY_SIZE - 1;
    
    real_idt_ptr.base = 0;
    real_idt_ptr.limit = 0x3ff;
}

void bios32_service(uint8_t interrupt_num, registers16_t *in_reg, registers16_t *out_reg)
{
    void *new_code_base = (void*)(REBASE_ADDRESS);

    memcpy((uint8_t*)(&_gdt_entries), (uint8_t*)(pGDT), sizeof(pGDT));

    real_gdt_ptr.base = (uint32_t)(REBASE((&_gdt_entries)));
    memcpy((uint8_t*)(&_gdt_ptr), (uint8_t*)(&real_gdt_ptr), sizeof(real_gdt_ptr));

    memcpy((uint8_t*)(&_idt_ptr), (uint8_t*)(&real_idt_ptr), sizeof(real_idt_ptr));

    memcpy((uint8_t*)(&_in_reg_ptr), (uint8_t*)(in_reg), sizeof(registers16_t));
    void *t = REBASE(&_in_reg_ptr);
    memcpy((uint8_t*)(&_intnum_ptr), (uint8_t*)(&interrupt_num), sizeof(uint8_t));

    uint32_t size = (uint32_t)(_bios32_helper_end) - (uint32_t)(_bios32_helper);
    memcpy((uint8_t*)(new_code_base), (uint8_t*)(_bios32_helper), size);

    rebased_bios32_helper();

    t = REBASE(&_out_reg_ptr);
    memcpy((uint8_t*)(out_reg), (uint8_t*)(t), sizeof(registers16_t));

    INTERRUPT_SAFE_BLOCK({
        restore();
    });
}

static void restore(void)
{
    kernel_global_descriptor_table_init();
    bios32_init();
    kernel_global_descriptor_table_install();

    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
}