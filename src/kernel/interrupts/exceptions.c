/// ------------------
/// @file exceptions.c

#include <stdint.h>
#include <data-structures/registers.h>

#include <dbgprintf.h>

void page_fault_handler(registers_t *r)
{
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

    uint32_t present = r->error & 0x1;
    uint32_t rw = r->error & 0x2;
    uint32_t user = r->error & 0x4;
    uint32_t reserved = r->error & 0x8;
    uint32_t inst_fetch = r->error & 0x10;

    dbgprintf("Page Fault | Possible causes: [ ");
    if (!present) dbgprintf("Page not present ");
    if (rw) dbgprintf("Page is read only ");
    if (user) dbgprintf("Page is read only ");
    if (reserved) dbgprintf("Overwrote reserved bits ");
    if (inst_fetch) dbgprintf("Instruction fetch ");
    dbgprintf("] at 0x%X\n", faulting_address);

    dbgprintf("Registers:\n");
    dbgprintf("EAX=0x%X EBX=0x%X ECX=0x%X EDX=0x%X\n",
              r->eax, r->ebx, r->ecx, r->edx);
    dbgprintf("ESI=0x%X EDI=0x%X EBP=0x%X ESP=0x%X\n",
              r->esi, r->edi, r->ebp, r->esp);
    dbgprintf("EIP=0x%X CS=0x%X DS=0x%X\n",
              r->eip, r->cs, r->ds);

}

void general_protection_fault_handler(registers_t *r)
{
    uint32_t error_code = r->error;
    uint32_t external = error_code & 0x1;
    uint32_t table = (error_code >> 1) & 0x3;
    uint32_t index = (error_code >> 3);

    dbgprintf("General Protection Fault | Error code: 0x%X\n", error_code);
    dbgprintf("Details: [ ");
    if (external) dbgprintf("External event ");
    switch (table)
    {
        case 0: dbgprintf("GDT "); break;
        case 1: dbgprintf("IDT "); break;
        case 2: dbgprintf("LDT "); break;
        default: dbgprintf("Unknown table "); break;
    }
    dbgprintf("Index: %D ]\n", index);
    dbgprintf("EIP at fault: 0x%X\n", r->eip);
}
