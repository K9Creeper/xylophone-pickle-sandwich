.global _task_save_context 
_task_save_context:
    /* preserve the current context */
    pushl %ebp
    movl %esp, %ebp

    pushl %eax

    movl 12(%esp), %eax  /* (+8) task_t */

    pushl %ebx

    movl %ebx, 12(%eax) /* ebx */
    movl %ecx, 4(%eax) /* ecx */
    movl %edx, 8(%eax) /* edx */
    movl %esi, 24(%eax) /* esi */
    movl %edi, 28(%eax) /* edi */
    movl %esp, 16(%eax) /* esp */
    movl %ebp, 20(%eax) /* ebp */

    pushfl
    popl %ebx
    movl %ebx, 36(%eax) /* eflags */
    fnsave 40(%eax) /* fpu_state */

    popl %ebx

    popl %eax

    movl %ebp, %esp
    popl %ebp

    ret

.global _task_restore_context 
_task_restore_context:
    /* preserve the current context */
    pushl %ebp
    movl %esp, %ebp

    pushl %eax

    movl 12(%esp), %eax /* (+8) task_t */


    movl 20(%eax), %ebp  /* ebp */
    movl 16(%eax), %esp  /* esp */

    frstor 40(%eax)  /* fpu_state */

    movl 12(%eax), %ebx  /* ebx */
    movl 4(%eax), %ecx   /* ecx */
    movl 8(%eax), %edx   /* edx */
    movl 24(%eax), %esi  /* esi */
    movl 28(%eax), %edi  /* edi */

    pushl 36(%eax)  /* eflags */
    popfl

    popl %eax

    movl %ebp, %esp
    popl %ebp

    ret

.global _task_start
_task_start:
    movl 4(%esp), %eax /* task_t */

    movl 164(%eax), %esp /* kesp */

    movl 148(%eax), %ebx /* args */
    movl 152(%eax), %ecx /* argv */
    movl 172(%eax), %edx  /* thread_eip */

    cmpl $0, 20(%eax) /* ebp */
    je _start_pcb_skip
    pushl	156(%eax) /* ds */
    pushl	16(%eax) /* esp */
_start_pcb_skip:
    pushl	$512 /* 0x200 eflag */
    pushl	160(%eax) /* cs */
    pushl	32(%eax)  /* eip */

    movw	156(%eax),%ds /* ds */
    movw	156(%eax),%es /* ds */

    iret
