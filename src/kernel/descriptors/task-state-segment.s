### --------------------------
### @file task-state-segment.s

.global _tss_flush
_tss_flush:
    mov $0x28, %ax
    ltr %ax
    ret
