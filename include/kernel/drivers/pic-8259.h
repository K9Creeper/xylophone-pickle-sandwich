/// -----------
/// @file pic-8259.h

#ifndef PIC_8259_H
#define PIC_8259_H

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define PIC_MASTER_OFFSET 0x20
#define PIC_SLAVE_OFFSET 0x28

#endif