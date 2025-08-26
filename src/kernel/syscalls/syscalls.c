/// ----------------
/// @file syscalls.h

#include "syscalls.h"

#include <kernel/data-structures/syscalls.h>

#include "../interrupts/interrupts.h"

#include <memory.h>

// linker.ld
extern const syscall_entry_t __start_syscalls[];
extern const syscall_entry_t __stop_syscalls[];

void* syscalls_table[SYSCALLS_MAX_COUNT];

// syscalls.s
extern void _handle_syscalls_interrupt(registers_t *frame);

void syscalls_init(void) {
    memset((uint8_t *)syscalls_table, 0, sizeof(syscalls_table));

    for (const syscall_entry_t* e = __start_syscalls; e < __stop_syscalls; ++e) {
        syscalls_table[e->num] = e->func;
    }

    kernel_interrupts_add_service_handle(0x80, _handle_syscalls_interrupt);
}