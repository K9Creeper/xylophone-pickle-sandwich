/// ----------------
/// @file syscalls.s

.extern syscalls_table/* void** */
.global _handle_syscalls_interrupt
_handle_syscalls_interrupt:
    movl 4(%esp), %edx        /* registers_t* */
    movl %edx, %esi

    movl 32(%edx), %ebx       /* eax syscall number */

    pushl 4(%edx)             /* edi */
    pushl 8(%edx)             /* esi */
    pushl 24(%edx)            /* edx */
    pushl 28(%edx)            /* ecx */
    pushl 20(%edx)            /* ebx */

    movl syscalls_table(, %ebx, 4), %ecx
    call *%ecx

    add $20, %esp             /* clean up stack, 5 * 4 bytes */

    movl %eax, 32(%esi)       /* store return value */

    ret
