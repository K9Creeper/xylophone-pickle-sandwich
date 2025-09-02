;;; ----------------
;;; @file bios32.asm
;;; @brief This file handles the switch from protected mode to real mode. It allows BIOS interrupts given by the C/C++ code. 
;;; This code came from https://github.com/szhou42/osdev/tree/master ( and relating content in this folder came from szhou42), sorry!.

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

global _return_stack

section .bios32_data

%define GDTENTRY(x)                            ((x) << 3)
%define CODE32                                 GDTENTRY(1)  ; 0x08
%define DATA32                                 GDTENTRY(2)  ; 0x10
%define CODE16                                 GDTENTRY(6)  ; 0x30
%define DATA16                                 GDTENTRY(7)  ; 0x38

PG_BIT_OFF equ 0x7fffffff
PG_BIT_ON equ 0x80000000

_bios32_helper: use32
    pusha
    mov edx, esp
    ; Now in 32bit protected mode
    ; Disable interrupts
    cli
    ; Turn off paging
    mov ecx, cr0
    and ecx, PG_BIT_OFF
    mov cr0, ecx
    ; Zero cr3(save it in ebx before zeroing it)
    xor ecx, ecx
    mov ebx, cr3
    mov cr3, ecx

    ; Load new gdt
    lgdt [_gdt_ptr]

    ; Load idt
    lidt [_idt_ptr]
   
    jmp CODE16:protected_mode_16
protected_mode_16:use16
    ; Now in 16bit protected mode
    ; Update data segment selector
    mov ax, DATA16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Turn off protected mode
    mov eax, cr0
    and  al,  ~0x01
    mov cr0, eax

    jmp 0x0:real_mode_16
real_mode_16:use16
    ; 16 bit real mode data segment
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, temp_esp_2 + 512

    sti

    ; ### Save current context ###
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

    ; ### Load the given context from _in_reg_ptr ###
    ; Temporarily change esp to _in_reg_ptr
    mov esp, _in_reg_ptr

    mov ax, _return_stack + 1024   ; get the address of _return_stack
    mov dx, ax                     ; copy for DI (offset)
    shr ax, 4                      ; segment = address >> 4

    mov [esp], dx                   ; in_reg->di (offset 0x00)
    mov [esp + 16], ax              ; in_reg->es (segment 0x10)

    ; only use some general register from the given context
    popa

    ; set a new stack for bios interrupt
    mov sp, temp_esp_3 + 512

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

    ; ### Restore current context ###
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
    
    cli
    
    jmp CODE32:protected_mode_32
protected_mode_32:use32
    mov ax, DATA32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; restore cr3
    mov cr3, ebx

    ; Turn on paging
    mov ecx, cr0
    or ecx, PG_BIT_ON
    mov cr0, ecx
    
    ; restore esp
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
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
    dd 0xaaaaaaaa
temp_esp:
    times 256 db 0
temp_esp_2:
    times 512 db 0
temp_esp_3:
    times 512 db 0
_return_stack:
    times 1028 db 0
_bios32_helper_end:
