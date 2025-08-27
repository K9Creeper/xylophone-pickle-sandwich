/// ------------
/// @file pic-8259.c

#include "pic-8259.h"

#include <memory.h>

#define ICW1_ICW4 0x01      /* Indicates that ICW4 will be present */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

#define CASCADE_IRQ 2

void pic_8259_remap(void)
{
    outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    io_wait();
    outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outportb(PIC1_DATA, PIC_MASTER_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    outportb(PIC2_DATA, PIC_SLAVE_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    outportb(PIC1_DATA, 1 << CASCADE_IRQ); // ICW3: tell Master PIC that there is a slave PIC at IRQ2
    io_wait();
    outportb(PIC2_DATA, 2); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outportb(PIC1_DATA, ICW4_8086); // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    io_wait();
    outportb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Unmask both PICs.
    outportb(PIC1_DATA, 0);
    outportb(PIC2_DATA, 0);
}

void pic_8259_disable(void){
    outportb(PIC1_DATA, 0xff);
    outportb(PIC2_DATA, 0xff);
}