/// ---------------
/// @file ata-pio.h

#ifndef _ATA_PIO_H
#define _ATA_PIO_H

#include <stdint.h>

#include "kernel/drivers/pci.h"
#include "kernel/data-structures/drivers/ide-controller.h"

#include "kernel/data-structures/drivers/ata.h"

extern uint8_t ata_pio_init_drive(const pci_device_t *dev, uint8_t drive, ide_device_t *dev_out);

extern void ata_pio_select_drive(uint8_t channel, uint8_t drive);

extern uint8_t ata_pio_status_wait(int32_t io_base, int32_t timeout);
extern void ata_pio_io_wait(int32_t io_base);
extern int32_t ata_pio_wait(int32_t io, uint8_t adv);

extern int32_t ata_pio_read_sector(int32_t io_base, uint8_t drive, uint64_t lba, void *buf);
extern int32_t ata_pio_write_sector(int32_t io_base, uint8_t drive, uint64_t lba, const void *buf);

#endif