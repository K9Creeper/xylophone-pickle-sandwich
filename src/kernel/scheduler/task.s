### ------------
### @file task.s

.section .text

.global _task_start
_task_start:
    movl 4(%esp), %eax /* task_t */

    movl 164(%eax), %esp /* kesp */

    movl 148(%eax), %ebx /* args */
    movl 152(%eax), %ecx /* argv */
    movl 172(%eax), %edx  /* thread_eip */

    cmpl $0, 20(%eax) /* ebp */
    je _skip
    pushl	156(%eax) /* ds */
    pushl	16(%eax) /* esp */
_skip:
    pushl	$0x200  /* eflag */
    pushl	160(%eax) /* cs */
    pushl	32(%eax)  /* eip */

    movw	156(%eax),%ds /* ds */
    movw	156(%eax),%es /* ds */

    iret
