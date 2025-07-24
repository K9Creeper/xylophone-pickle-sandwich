### ----------------------------
### interrupt-descriptor-table.s
### @brief This file defines a core function for the Interrupt Descriptor Table.

.global _idt_load
.extern _pIDT
  
_idt_load:
    lidt [ _pIDT ]
    ret
