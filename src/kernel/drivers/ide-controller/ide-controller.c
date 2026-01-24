/// ----------------------
/// @file ide-controller.c

#include "kernel/data-structures/drivers/drive-controller.h"
#include "kernel/data-structures/drivers/ide-controller.h"

#include "kernel/drivers/pci.h"
#include "kernel/drivers/ata-pio.h"

#define MAX_IDE_DEVICES 4

static ide_device_t ide_devices[MAX_IDE_DEVICES] = {0};
static uint8_t ide_drive_count = 0;

static int32_t ide_read(void *impl, uint64_t lba, uint32_t sector_count, void *buffer);
static int32_t ide_write(void *impl, uint64_t lba, uint32_t sector_count, const void *buffer);
static uint32_t ide_sector_size(void *impl);
static uint64_t ide_sector_count(void *impl);

static const drive_controller_ops_t ide_ops = {
    .read = ide_read,
    .write = ide_write,
    .sector_size = ide_sector_size,
    .sector_count = ide_sector_count,
};

static int32_t ide_read(void *impl, uint64_t lba, uint32_t sector_count, void *buffer)
{
    const ide_device_t *dev = (const ide_device_t *)impl;
    uint16_t io_base = (dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    uint8_t *buf8 = (uint8_t *)buffer;
    for (uint32_t i = 0; i < sector_count; i++)
    {
        if (ata_pio_read_sector(io_base, dev->drive, lba + i, buf8 + i * 512) != 0)
            return -1;
    }

    return sector_count;
}

static int32_t ide_write(void *impl, uint64_t lba, uint32_t sector_count, const void *buffer)
{
    const ide_device_t *dev = (const ide_device_t *)impl;
    uint16_t io_base = (dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;

    const uint8_t *buf8 = (const uint8_t *)buffer;
    for (uint32_t i = 0; i < sector_count; i++)
    {
        if (ata_pio_write_sector(io_base, dev->drive, lba + i, buf8 + i * 512) != 0)
            return -1;
    }

    return sector_count;
}

static uint32_t ide_sector_size(void *impl)
{
    const ide_device_t *dev = (const ide_device_t *)impl;
    return dev->sector_size;
}

static uint64_t ide_sector_count(void *impl)
{
    const ide_device_t *dev = (const ide_device_t *)impl;
    return dev->sector_count;
}

#include <kernel/dbgprintf.h>

static uint8_t ide_pci_probe(const pci_device_t *pci_dev)
{
    if (pci_dev->header.type0.common.class_code != 0x1 ||
        pci_dev->header.type0.common.subclass_code != 0x1)
        return 1;

    for (uint8_t channel = 0; channel <= ATA_SECONDARY; channel++)
    {
        for (uint8_t drive = ATA_MASTER; drive <= ATA_SLAVE; drive++)
        {
            uint8_t dev_idx = (channel << 1) | drive;

            if (ata_pio_init_drive(pci_dev, drive, &ide_devices[dev_idx]) != 0)
                continue;

            static const char *channel_names[] = {"Primary", "Secondary"};
            static const char *drive_names[] = {"Master", "Slave"};

            const char *name = ide_devices[dev_idx].model;
            void *impl = (void *)&ide_devices[dev_idx];
            const drive_controller_ops_t *ops = &ide_ops;
            const uint32_t flags = 0;

            // Create a driver_controller
        }
    }

    return 0;
}

REGISTER_PCI_DRIVER(
    PCI_ANY_ID,
    PCI_ANY_ID,
    PCI_ANY_CLASS,
    PCI_ANY_SUBCLASS,
    "Generic IDE Controller",
    ide_pci_probe,
    NULL);