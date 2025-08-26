### ------------------
### @file interrupts.s

.macro IR_NO_ERR index
	.global ir\index
	ir\index:
		cli
        pushl $0
		pushl $\index
		jmp ir_common_stub
.endm

.macro IR_ERR index
	.global ir\index
	ir\index:
		cli
		pushl $\index
		jmp ir_common_stub
.endm

IR_NO_ERR 0
IR_NO_ERR 1
IR_NO_ERR 2
IR_NO_ERR 3
IR_NO_ERR 4
IR_NO_ERR 5
IR_NO_ERR 6
IR_NO_ERR 7
IR_ERR 8
IR_NO_ERR 9
IR_ERR 10
IR_ERR 11
IR_ERR 12
IR_ERR 13
IR_ERR 14
IR_NO_ERR 15
IR_NO_ERR 16
IR_ERR 17
IR_NO_ERR 18
IR_NO_ERR 19
IR_NO_ERR 20
IR_ERR 21
IR_NO_ERR 22
IR_NO_ERR 23
IR_NO_ERR 24
IR_NO_ERR 25
IR_NO_ERR 26
IR_NO_ERR 27
IR_NO_ERR 28
IR_NO_ERR 29
IR_NO_ERR 30
IR_NO_ERR 31
IR_NO_ERR 32
IR_NO_ERR 33
IR_NO_ERR 34
IR_NO_ERR 35
IR_NO_ERR 36
IR_NO_ERR 37
IR_NO_ERR 38
IR_NO_ERR 39
IR_NO_ERR 40
IR_NO_ERR 41
IR_NO_ERR 42
IR_NO_ERR 43
IR_NO_ERR 44
IR_NO_ERR 45
IR_NO_ERR 46
IR_NO_ERR 47

IR_NO_ERR 128

## interrupts.c
.extern ir_handler
ir_common_stub:
    pusha
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    call ir_handler
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popa
    addl $8, %esp

    iret
