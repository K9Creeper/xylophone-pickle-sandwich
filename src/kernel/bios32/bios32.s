### ------------
### @file bios32.s

.code32

.section .text

.global _bios32_helper
.global _bios32_helper_end
 
.global _gdt_ptr
.global _gdt_entries
.global _idt_ptr
.global _in_reg_ptr
.global _out_reg_ptr
.global _intnum_ptr

.set PG_BIT_OFF, 0x7fffffff
.set PG_BIT_ON, 0x80000000

.section .bios32_data, "ax", @progbits
.balign 8
_bios32_helper: 
    pushal
    movl %esp, %edx

    cli

    movl %cr0, %ecx
    andl $PG_BIT_OFF, %ecx
    movl %ecx, %cr0

    xorl %ecx, %ecx
    movl %cr3, %ebx
    movl %ecx, %cr3

    lgdt ( _gdt_ptr )
    lidt ( _idt_ptr )

    jmp $0x30,$protected_mode_16
protected_mode_16:
    .code16

    movw $0x38, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    .code32
    movl %cr0, %eax
    andb $~0x01, %al
    movl %eax, %cr0
    .code16

    jmp $0x0,$real_mode_16
real_mode_16:
    .code16

    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    movw $temp_esp_2, %sp

    sti

    pusha
    movw %ss, %cx
    pushw %cx
    movw %gs, %cx
    pushw %cx
    movw %fs, %cx
    pushw %cx
    movw %es, %cx
    pushw %cx
    movw %ds, %cx
    pushw %cx
    pushf

    movw %sp, %ax
    movw $sp_snapshot, %di
    stosw

    movw $_in_reg_ptr, %sp
    movw %sp, %si
    movw $_return_stack, 0(%si)
    movw $0x0, 16(%si)

    popa

    movw $temp_esp_3, %sp

    .byte 0xCD
_intnum_ptr:
    .byte 0x0

    movw $_out_reg_ptr_top, %sp
    pushf
    movw %ss, %cx
    pushw %cx
    movw %gs, %cx
    pushw %cx
    movw %fs, %cx
    pushw %cx
    movw %es, %cx
    pushw %cx
    movw %ds, %cx
    pushw %cx
    pusha

    movw $sp_snapshot, %si
    lodsw
    movw %ax, %sp

    popf
    popw %cx
    movw %cx, %ds
    popw %cx
    movw %cx, %es
    popw %cx
    movw %cx, %fs
    popw %cx
    movw %cx, %gs
    popw %cx
    movw %cx, %ss
    popa

    cli

    .code32
    movl %cr0, %eax
    inc %eax
    movl %eax, %cr0
    .code16

    jmp $0x08,$protected_mode_32
protected_mode_32:
    .code32

    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movl %ebx, %cr3

    movl %cr0, %ecx
    orl $PG_BIT_ON, %ecx
    movl %ecx, %cr0

    movl %edx, %esp

    popa

    ret
.balign 8
_gdt_entries:
    .skip 64
_gdt_ptr:
    .long 0x00000000
    .long 0x00000000
_idt_ptr:
    .long 0x00000000
    .long 0x00000000
#_intnum_ptr:
#    .byte 0x0
_in_reg_ptr:
    .skip 28
_out_reg_ptr:
    .space 28, 0xaa
_out_reg_ptr_top:

sp_snapshot:
    .word 0x00
temp_esp_2_bottom:
    .skip 512
temp_esp_2:

temp_esp_3_bottom:
    .skip 512
temp_esp_3:

_return_stack_bottom:
    .skip 512
_return_stack:

_bios32_helper_end:
