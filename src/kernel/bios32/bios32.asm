;;; ----------------
;;; @file bios32.asm

[bits 32]

section .text
global _bios32_helper
global _bios32_helper_end

global _gdt_ptr
global _gdt_entries
global _idt_ptr
global _in_reg_ptr
global _out_reg_ptr
global _intnum_ptr

section .bios32_data

%define CODE32                                 GDTENTRY(1)  ; 0x08
%define DATA32                                 GDTENTRY(2)  ; 0x10
%define CODE16                                 GDTENTRY(6)  ; 0x30
%define DATA16                                 GDTENTRY(7)  ; 0x38

PG_BIT_OFF equ 0x7fffffff
PG_BIT_ON equ 0x80000000

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

    lgdt [_gdt_ptr]

    lidt [_idt_ptr]
   
    jmp 0x30:protected_mode_16
protected_mode_16:use16
    mov ax, 0x38
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov eax, cr0
    and  al,  ~0x01
    mov cr0, eax

    jmp 0x0:real_mode_16
real_mode_16:use16
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, temp_esp_2

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
    mov edi, sp_snapshot
    stosw

    mov esp, _in_reg_ptr

    mov eax, _return_stack
    mov [esp], eax       

    mov ax, _return_stack
    mov [esp + 16], ax

    popa

    mov sp, temp_esp_3

    db 0xCD
_intnum_ptr:
    db 0x00
    
    mov esp, _out_reg_ptr
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

    mov esi, sp_snapshot
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
    
    cli
    
    jmp 0x08:protected_mode_32
protected_mode_32:use32
    mov ax, 0x10
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
    
    popa

    ret
padding:
    db 0x0
    db 0x0
    db 0x0
_gdt_entries:
    times 64 db 0
_gdt_ptr:
    dd 0x00000000
    dd 0x00000000
_idt_ptr:
    dd 0x00000000
    dd 0x00000000
_in_reg_ptr:
    times 14*2 db 0
_out_reg_ptr:
    times 14*2 db 0xaa
sp_snapshot:
    dw 0x00
temp_esp_2_bottom:
    times 512 db 0
temp_esp_2:

temp_esp_3_bottom:
    times 512 db 0
temp_esp_3:

_return_stack_bottom:
    times 1028 db 0
_return_stack:

_bios32_helper_end: