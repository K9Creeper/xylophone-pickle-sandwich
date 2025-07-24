/// --------
/// bios32.c
/// @brief This file defines the functions to access and call BIOS functions.

#include "bios32.h"

#include <data-structures/kernel-descriptors/global-descriptor-table.h>
#include <data-structures/kernel-descriptors/interrupt-descriptor-table.h>

#include "../descriptors/global-descriptor-table.h"
#include "../descriptors/interrupt-descriptor-table.h"

#include <memory.h>

#define REBASED_OFFSET 0x7C00

void (*rebased__bios32_helper)(void) = (void(*)(void))(REBASED_OFFSET);

// bios32.asm
extern void _bios32_helper(void);
extern void _bios32_helper_end(void);

#define REBASE(x) (void*)(REBASED_OFFSET + (uint32_t)(x) - (uint32_t)(_bios32_helper))

// bios32.asm
extern void *_gdt_entries;
extern void *_gdt_ptr;
extern void *_idt_ptr;
extern void *_in_reg_ptr;
extern void *_out_reg_ptr;
extern void *_intnum_ptr;

interrupt_descriptor_table_ptr_t rIDTptr;
global_descriptor_table_ptr_t rGDTptr;

// ../desciptors/global-descriptor-table.c
extern global_descriptor_table_entry_t* pGDT;
extern void kernel_global_descriptor_set_gate(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

#define GDT_ACCESS_CODE_SEGMENT  0x9A
#define GDT_ACCESS_DATA_SEGMENT  0x92
#define GDT_GRANULARITY_FLAGS    0x0F
#define IDT_LIMIT                0x3FF

static void restore();

void kernel_bios32_init(void){
    kernel_global_descriptor_set_gate(6, 0, 0xFFFFFFFF, GDT_ACCESS_CODE_SEGMENT, GDT_GRANULARITY_FLAGS);
    kernel_global_descriptor_set_gate(7, 0, 0xFFFFFFFF, GDT_ACCESS_DATA_SEGMENT, GDT_GRANULARITY_FLAGS);

    rGDTptr.base = (uint32_t)(pGDT);
    rGDTptr.limit = sizeof(pGDT) - 1;

    rIDTptr.base = 0;
    rIDTptr.limit = IDT_LIMIT;
}

void kernel_bios32_call(uint8_t int_num, registers16_t * in_reg, registers16_t* out_reg, bool enable_int){
    void *new_code_base = (void*)(REBASED_OFFSET);

    memcpy((uint8_t*)(&_gdt_entries), (uint8_t*)(pGDT), sizeof(pGDT));

    rGDTptr.base = (uint32_t)(REBASE((&_gdt_entries)));
    memcpy((uint8_t*)(&_gdt_ptr), (uint8_t*)(&rGDTptr), sizeof(rGDTptr));

    memcpy((uint8_t*)(&_idt_ptr), (uint8_t*)(&rIDTptr), sizeof(rIDTptr));

    memcpy((uint8_t*)(&_in_reg_ptr), (uint8_t*)(in_reg), sizeof(registers16_t));
    void *t = REBASE(&_in_reg_ptr);
    memcpy((uint8_t*)(&_intnum_ptr), (uint8_t*)(&int_num), sizeof(uint8_t));

    uint32_t size = (uint32_t)(_bios32_helper_end) - (uint32_t)(_bios32_helper);
    memcpy((uint8_t*)(new_code_base), (uint8_t*)(_bios32_helper), size);

    rebased__bios32_helper();

    t = REBASE(&_out_reg_ptr);
    memcpy((uint8_t*)(out_reg), (uint8_t*)(t), sizeof(registers16_t));

    asm volatile("cli");

    restore();

    if(enable_int)
        asm volatile("sti");
}

static void restore(){
    kernel_global_descriptor_table_init();
    kernel_bios32_init();
    kernel_global_descriptor_table_install();

    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
}