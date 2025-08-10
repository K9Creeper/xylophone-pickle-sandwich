### --------------------
### task-state-segment.s
### @brief This file defines the functions needed to set up the Task State Segment.

.global _tss_flush
_tss_flush:
    mov $0x28, %ax
    ltr %ax
    ret
