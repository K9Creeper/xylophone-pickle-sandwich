/// ------------
/// @file cmos.h

#ifndef CMOS_H
#define CMOS_H

#include <memory.h>
#include <kernel/util.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

static inline void read_from_cmos(unsigned char array[])
{
    unsigned char tvalue, index;
    INTERRUPT_SAFE_BLOCK({
        for (index = 0; index < 128; index++)
        {
            outportb(CMOS_ADDRESS, index);
            io_wait();
            array[index] = inportb(CMOS_DATA);
            io_wait();
        }
    });
}

static inline void write_from_cmos(unsigned char array[])
{
    unsigned char index;
    INTERRUPT_SAFE_BLOCK({
        for (index = 0; index < 128; index++){
            unsigned char tvalue = array[index];
            outportb(CMOS_ADDRESS, index);
            io_wait();
            outportb(CMOS_DATA, tvalue);
            io_wait();
        }
    });
}

#endif