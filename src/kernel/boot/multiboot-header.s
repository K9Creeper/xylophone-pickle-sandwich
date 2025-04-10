.equ MBOOT2_MAGIC, 0xE85250D6
.equ MBOOT2_ARCH, 0
.equ MBOOT2_LENGTH, (multiboot2HeaderEnd - multiboot2Header)
.equ MBOOT2_CHECKSUM, -(MBOOT2_MAGIC + MBOOT2_ARCH + MBOOT2_LENGTH)

.section .multiboot
.align 8

multiboot2Header:
.long MBOOT2_MAGIC
.long MBOOT2_ARCH
.long MBOOT2_LENGTH
.long MBOOT2_CHECKSUM

.align 8

.short 0
.short 0
.short 8
multiboot2HeaderEnd:
