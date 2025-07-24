;;; ----------
;;; bios32.asm
;;; @brief This file handles the switch from protected mode to real mode. It allows BIOS interrupts given by the C/C++ code. 
;;; This code came from https://github.com/szhou42/osdev/tree/master ( and relating content in this folder came from szhou42), sorry!.

[bits 32]

global _bios32_helper
global _bios32_helper_end

global _gdt_ptr
global _gdt_entries
global _idt_ptr
global _in_reg_ptr
global _out_reg_ptr
global _intnum_ptr


extern new_gdt_entries;
extern new_gdt_ptr;
extern new_idt_ptr;
extern new_reg_ptr;
extern new_intnum_ptr;

%define REBASED_OFFSET                         0x7C00

%define REBASE(x)                              (((x) - _bios32_helper) + REBASED_OFFSET)
%define GDTENTRY(x)                            ((x) << 3)
%define CODE32                                 GDTENTRY(1)  ; 0x08
%define DATA32                                 GDTENTRY(2)  ; 0x10
%define CODE16                                 GDTENTRY(6)  ; 0x30
%define DATA16                                 GDTENTRY(7)  ; 0x38


PG_BIT_OFF equ 0x7fffffff
PG_BIT_ON equ 0x80000000

section .text
_bios32_helper: use32
    pusha
    mov edx, esp
    
    cli

    mov ecx, cr0
    and ecx, PG_BIT_OFF
    mov cr0, ecx

    xor ecx, ecx
    mov ebx, cr3
    mov cr3, ecx


    lgdt [REBASE(_gdt_ptr)]


    lidt [REBASE(_idt_ptr)]
   
    jmp CODE16:REBASE(protected_mode_16)
protected_mode_16:use16

    mov ax, DATA16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and  al,  ~0x01
    mov cr0, eax

    jmp 0x0:REBASE(real_mode_16)
real_mode_16:use16
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x8c00

    sti

    pusha
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pushf

    mov ax, sp
    mov edi, temp_esp
    stosw

    mov esp, REBASE(_in_reg_ptr)

    popa

    mov sp, 0x9c00

    db 0xCD
_intnum_ptr:

    db 0x00

    mov esp, REBASE(_out_reg_ptr)
    add sp, 28

    pushf
    mov cx, ss
    push cx
    mov cx, gs
    push cx
    mov cx, fs
    push cx
    mov cx, es
    push cx
    mov cx, ds
    push cx
    pusha


    mov esi, temp_esp
    lodsw
    mov sp, ax

    popf
    pop cx
    mov ds, cx
    pop cx
    mov es, cx
    pop cx
    mov fs, cx
    pop cx
    mov gs, cx
    pop cx
    mov ss, cx
    popa

    mov eax, cr0
    inc eax
    mov cr0, eax
    jmp CODE32:REBASE(protected_mode_32)
protected_mode_32:use32
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov cr3, ebx

    mov ecx, cr0
    or ecx, PG_BIT_ON
    mov cr0, ecx
    
    mov esp, edx

    sti
    popa
    ret

padding:
    db 0x0
    db 0x0
    db 0x0
_gdt_entries:
    ; 8 gdt entries
    resb 64
_gdt_ptr:
    dd 0x00000000
    dd 0x00000000
_idt_ptr:
    dd 0x00000000
    dd 0x00000000
_in_reg_ptr:
    resw 14
_out_reg_ptr:
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
temp_esp:
    dw 0x0000

_bios32_helper_end:
