### ---------------------
### interrupt-service.s
### @brief This file defines the core functions for handling internal interrupts.

.global isr0
isr0:
    cli
    pushl $0
    pushl $0
    jmp isr_common_stub

.global isr1
isr1:
    cli
    pushl $0
    pushl $1
    jmp isr_common_stub

.global isr2
isr2:
    cli
    pushl $0
    pushl $2
    jmp isr_common_stub

.global isr3
isr3:
    cli
    pushl $0
    pushl $3
    jmp isr_common_stub

.global isr4
isr4:
    cli
    pushl $0
    pushl $4
    jmp isr_common_stub

.global isr5
isr5:
    cli
    pushl $0
    pushl $5
    jmp isr_common_stub

.global isr6
isr6:
    cli
    pushl $0
    pushl $6
    jmp isr_common_stub

.global isr7
isr7:
    cli
    pushl $0
    pushl $7
    jmp isr_common_stub

.global isr8
isr8:
    cli
    pushl $8
    jmp isr_common_stub

.global isr9
isr9:
    cli
    pushl $0
    pushl $9
    jmp isr_common_stub

.global isr10
isr10:
    cli
    pushl $10
    jmp isr_common_stub

.global isr11
isr11:
    cli
    pushl $11
    jmp isr_common_stub

.global isr12
isr12:
    cli
    pushl $12
    jmp isr_common_stub

.global isr13
isr13:
    cli
    pushl $13
    jmp isr_common_stub

.global isr14
isr14:
    cli
    pushl $14
    jmp isr_common_stub

.global isr15
isr15:
    cli
    pushl $0
    pushl $15
    jmp isr_common_stub

.global isr16
isr16:
    cli
    pushl $0
    pushl $16
    jmp isr_common_stub

.global isr17
isr17:
    cli
    pushl $0
    pushl $17
    jmp isr_common_stub

.global isr18
isr18:
    cli
    pushl $0
    pushl $18
    jmp isr_common_stub

.global isr19
isr19:
    cli
    pushl $0
    pushl $19
    jmp isr_common_stub

.global isr20
isr20:
    cli
    pushl $0
    pushl $20
    jmp isr_common_stub

.global isr21
isr21:
    cli
    pushl $0
    pushl $21
    jmp isr_common_stub

.global isr22
isr22:
    cli
    pushl $0
    pushl $22
    jmp isr_common_stub

.global isr23
isr23:
    cli
    pushl $0
    pushl $23
    jmp isr_common_stub

.global isr24
isr24:
    cli
    pushl $0
    pushl $24
    jmp isr_common_stub

.global isr25
isr25:
    cli
    pushl $0
    pushl $25
    jmp isr_common_stub

.global isr26
isr26:
    cli
    pushl $0
    pushl $26
    jmp isr_common_stub

.global isr27
isr27:
    cli
    pushl $0
    pushl $27
    jmp isr_common_stub

.global isr28
isr28:
    cli
    pushl $0
    pushl $28
    jmp isr_common_stub

.global isr29
isr29:
    cli
    pushl $0
    pushl $29
    jmp isr_common_stub

.global isr30
isr30:
    cli
    pushl $0
    pushl $30
    jmp isr_common_stub

.global isr31
isr31:
    cli
    pushl $0
    pushl $31
    jmp isr_common_stub

.extern isr_handler

isr_common_stub:
    pusha
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    call isr_handler
    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popa
    addl $8, %esp
    sti
    iret
