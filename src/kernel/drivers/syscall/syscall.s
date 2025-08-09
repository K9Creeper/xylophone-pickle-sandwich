.extern syscall_handlers /* void** */
.global _handle_syscall_interrupt
_handle_syscall_interrupt:
    movl 4(%esp), %edx        /* registers_t */

    movl 32(%edx), %ebx       /* index (eax) */

    pushl 4(%edx)             /* edi */
    pushl 8(%edx)             /* esi */
    pushl 24(%edx)            /* edx */
    pushl 28(%edx)            /* ecx */
    pushl 20(%edx)            /* ebx */

    movl syscall_handlers(, %ebx, 4), %ecx
    call *%ecx

    popl %ebx
    popl %ebx
    popl %ebx
    popl %ebx
    popl %ebx

    movl %eax, 32(%edx)       /* return value -> eax (registers_t) */

    ret
