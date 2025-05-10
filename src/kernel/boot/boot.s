### ------
### boot.s
### @brief This file handles the switch from GRUB to protected mode. It also sets up the kernel as a higher half.

.section .bss
stack_bottom:
    .skip 16384       # Reserve space for the stack (16KB)
stack_top:

.set KVIRT_BASE, 0xC0000000
.set KPAGE_NUM, (KVIRT_BASE >> 22)

.section .data
.align 4096
.global page_directory
page_directory:
    .long 0x00000083                            # Identity map the first 4MB
    .fill (KPAGE_NUM - 1), 4, 0x00000000
    .long 0x00000083                            # Map the higher half (initially same as first 4mb for jump)
    .fill (1024 - KPAGE_NUM - 1), 4, 0x00000000

.section .text

.extern kernel_main
.global _start
_start:
    cli

    # load page dir
    movl $(page_directory - KVIRT_BASE), %ecx
    movl %ecx, %cr3

    # enable 4mb paging
    movl %cr4, %ecx
    orl $0x00000010, %ecx # Set PSE bit
    movl %ecx, %cr4

    # enable paging
    movl %cr0, %ecx
    orl $0x80000000, %ecx # Set PG bit
    movl %ecx, %cr0

    # jump to higher half
    lea _entry_higher, %ecx
    jmp *%ecx

_entry_higher:
    # invalidate table
    movl $0x00000000, page_directory
    invlpg [0]

    movl $stack_top, %esp

    # Multiboot2 address
    push %eax
    # Multiboot2 magic
    push %ebx

    call kernel_main

    cli

.hang:
    hlt
    jmp .hang
