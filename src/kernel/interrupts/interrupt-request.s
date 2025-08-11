### -------------------
### interrupt-request.s
### @brief This file defines core functions to handle external (device) interrupts.

.global irq0
irq0:
    cli
    pushl $0
    pushl $32
    jmp irq_common_stub

.global irq1
irq1:
    cli
    pushl $0
    pushl $33
    jmp irq_common_stub

.global irq2
irq2:
    cli
    pushl $0
    pushl $34
    jmp irq_common_stub

.global irq3
irq3:
    cli
    pushl $0
    pushl $35
    jmp irq_common_stub

.global irq4
irq4:
    cli
    pushl $0
    pushl $36
    jmp irq_common_stub

.global irq5
irq5:
    cli
    pushl $0
    pushl $37
    jmp irq_common_stub

.global irq6
irq6:
    cli
    pushl $0
    pushl $38
    jmp irq_common_stub

.global irq7
irq7:
    cli
    pushl $0
    pushl $39
    jmp irq_common_stub

.global irq8
irq8:
    cli
    pushl $0
    pushl $40
    jmp irq_common_stub

.global irq9
irq9:
    cli
    pushl $0
    pushl $41
    jmp irq_common_stub

.global irq10
irq10:
    cli
    pushl $0
    pushl $42
    jmp irq_common_stub

.global irq11
irq11:
    cli
    pushl $0
    pushl $43
    jmp irq_common_stub

.global irq12
irq12:
    cli
    pushl $0
    pushl $44
    jmp irq_common_stub

.global irq13
irq13:
    cli
    pushl $0
    pushl $45
    jmp irq_common_stub

.global irq14
irq14:
    cli
    pushl $0
    pushl $46
    jmp irq_common_stub

.global irq15
irq15:
    cli
    pushl $0
    pushl $47
    jmp irq_common_stub

.extern irq_handler           

irq_common_stub:
    pusha                    

    movw %ds, %ax            
    pushl %eax               

    movw $0x10, %ax          
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call irq_handler

    popl %eax                
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    popa                     

    addl $8, %esp

    iret                     
