/// ----------------------
/// @file ide-controller.h

#ifndef DRIVERS_IDE_CONTROLLER_H
#define DRIVERS_IDE_CONTROLLER_H

#include <stdint.h>

#include <kernel/data-structures/storage/storage-controller.h>

typedef struct ide_device_s
{
    uint8_t  present;       
    uint8_t  channel;       
    uint8_t  drive;         
    uint16_t type;          

    uint16_t signature;      
    uint16_t capabilities;    
    uint32_t command_sets;  

    uint32_t sector_size;    
    uint64_t sector_count;
    
    char model[41];      

    uint8_t  identify_data[512];

    uint32_t controller_status;

    volatile uint8_t irq_expected;
    volatile uint8_t irq_fired;
} ide_device_t;

#endif