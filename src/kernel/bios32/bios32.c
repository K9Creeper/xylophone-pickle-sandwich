/// --------------
/// @file bios32.c
//
// TODO: Re-write this bs

#include "bios32.h"

#include <memory.h>
#include <kernel/util.h>

#include <kernel/data-structures/descriptors/global-descriptor-table.h>
#include <kernel/data-structures/descriptors/interrupt-descriptor-table.h>
#include "../descriptors/global-descriptor-table.h"
#include "../descriptors/interrupt-descriptor-table.h"

#include "../drivers/pic-8259/pic-8259.h"

#include <kernel/data-structures/kernel-context.h>
extern kernel_context_t *kernel_context;

#include <kernel/memory-management/paging-manager.h>

extern void _bios32_helper();
extern void _bios32_helper_end();

extern void _gdt_flush(void);

extern global_descriptor_table_entry_t _gdt_entries[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];
extern global_descriptor_table_ptr_t _gdt_ptr;
extern interrupt_descriptor_table_ptr_t _idt_ptr;
extern registers16_t _in_reg_ptr;
extern registers16_t _out_reg_ptr;
extern uint16_t _intnum_ptr;

extern void* sp_snapshot;

interrupt_descriptor_table_ptr_t real_idt_ptr;
global_descriptor_table_ptr_t real_gdt_ptr;

extern global_descriptor_table_entry_t pGDT[KERNEL_DESCRIPTORS_GDT_ENTRYCOUT];

uint8_t kernel_bios32_init(void)
{
    if (_bios32_helper_end > 0x8FF0 || _bios32_helper != 0x8000)
        return 1;

    paging_manager_identity_allocate_range(
        &kernel_context->paging_manager,
        (uint32_t)_bios32_helper,
        (uint32_t)_bios32_helper_end,
        1,
        1);

    memcpy((uint8_t *)(_gdt_entries), (uint8_t *)(pGDT), sizeof(pGDT));

    _gdt_ptr.base = (uint32_t)(_gdt_entries);
    _gdt_ptr.limit = sizeof(pGDT) - 1;

    _idt_ptr.base = 0x0;
    _idt_ptr.limit = 0x3ff;

    return 0;
}

void *kernel_bios32_service(uint8_t interrupt_num, registers16_t *in_reg, registers16_t *out_reg)
{
    memcpy((uint8_t *)(&_in_reg_ptr), (uint8_t *)(in_reg), sizeof(registers16_t));
    memcpy((uint8_t *)(&_intnum_ptr), (uint8_t *)(&interrupt_num), sizeof(uint8_t));

    DISABLE_INTERRUPTS();
    pic_8259_disable();

    _bios32_helper();

    memcpy((uint8_t *)(out_reg), (uint8_t *)(&_out_reg_ptr), sizeof(registers16_t));

    _gdt_flush();
    kernel_interrupt_descriptor_table_finalize();

    pic_8259_remap();

    ENABLE_INTERRUPTS();

    return (void *)out_reg->di;
}
