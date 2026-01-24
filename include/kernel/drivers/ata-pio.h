/// ---------------
/// @file ata-pio.h

#ifndef _ATA_PIO_H
#define _ATA_PIO_H

#include <stdint.h>

#include <kernel/drivers/pci.h>
#include <kernel/data-structures/drivers/ide-controller.h>

#include <kernel/data-structures/drivers/ata.h>

extern uint8_t ata_pio_init_drive(const pci_device_t *dev, uint8_t drive, ide_device_t *dev_out);

extern void ata_pio_clear_status(ide_device_t *dev);

extern ata_error_t ata_pio_select_drive(uint8_t channel, uint8_t drive);

extern ata_error_t ata_pio_read_sector(ide_device_t *dev, uint64_t lba, void *buf);
extern ata_error_t ata_pio_write_sector(ide_device_t *dev, uint64_t lba, const void *buf);

#endif