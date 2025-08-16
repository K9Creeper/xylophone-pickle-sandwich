/// -----
/// ata.h
/// @brief This file defines the structures and utilities for ata.

#ifndef ATA_H
#define ATA_H

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY      0x00
#define ATA_SECONDARY    0x01

#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_STATUS     0x07
#define ATA_REG_ALTSTATUS  0x0C

#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05

#define ATA_REG_COMMAND    0x07
#define ATA_REG_DATA       0x00
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_FEATURES   0x01

#define ATA_CMD_IDENTIFY          0xEC
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_WRITE_PIO         0x30

#define ATA_SR_BSY     0x80	   // BUSY
#define ATA_SR_ERR     0x01    // Error
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_DF      0x20    // Drive write fault


#define ATA_IDENT_DEVICETYPE   0

#define ATA_IDENT_MODEL        54
#define ATA_IDE_MODEL_LENGTH 41
#define ATA_IDENT_COMMANDSETS  164

#define ATA_IDENT_MAX_LBA_EXT  200
#define ATA_IDENT_MAX_LBA      120

#include <stdint.h>

typedef struct ide_device_s
{
    uint8_t reserved;
    uint8_t channel;
    uint8_t drive;
    uint16_t type;
    uint16_t signature;
    uint16_t capabilities;
    uint32_t command_sets;
    uint32_t size;
    uint8_t model[41];

    uint8_t driver_data[512];
} ide_device_t;

#endif