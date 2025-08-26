### -------------------------------
### @file global-descriptor-table.s

.global _gdt_flush
.extern _pGDT

_gdt_flush:
  lgdt [ _pGDT ]
  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs
  mov %ax, %ss
  jmp $0x08,$flush2 
flush2:
  nop
  ret
