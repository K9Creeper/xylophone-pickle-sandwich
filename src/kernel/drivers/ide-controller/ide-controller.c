/// ----------------------
/// @file ide-controller.c

#include <kernel/data-structures/storage/storage-controller.h>
#include <kernel/data-structures/drivers/ide-controller.h>

#include <kernel/drivers/pci.h>
#include <kernel/drivers/ata-pio.h>

#include "../../interrupts/interrupts.h"

#include <kernel/dbgprintf.h>

#include <kernel/util.h>
#include <kernel/storage/storage-manager.h>

#include <kernel/data-structures/kernel-context.h>
extern kernel_context_t *kernel_context;

#define MAX_IDE_DEVICES 4

static ide_device_t ide_devices[MAX_IDE_DEVICES] = {0};
static uint8_t ide_drive_count = 0;

static int32_t ide_read(void *impl, uint64_t lba, uint32_t sector_count, void *buffer);
static int32_t ide_write(void *impl, uint64_t lba, uint32_t sector_count, const void *buffer);
static uint32_t ide_sector_size(void *impl);
static uint64_t ide_sector_count(void *impl);
static uint32_t ide_status(void *impl);

static const storage_controller_ops_t ide_ops = {
    .read = ide_read,
    .write = ide_write,
    .sector_size = ide_sector_size,
    .sector_count = ide_sector_count,
    .status = ide_status};

static void ata_primary_irq_handle(registers_t *regs)
{
    for (int drive = ATA_MASTER; drive <= ATA_SLAVE; drive++)
    {
        ide_device_t *dev = &ide_devices[(ATA_PRIMARY << 1) | drive];
        if (!dev->irq_expected)
            continue;

        dev->irq_fired = 1;
        ata_pio_clear_status(dev);
    }

    IR_EOI(ATA_PRIMARY_IRQ);
}

static void ata_secondary_irq_handle(registers_t *regs)
{
    for (int drive = ATA_MASTER; drive <= ATA_SLAVE; drive++)
    {
        ide_device_t *dev = &ide_devices[(ATA_SECONDARY << 1) | drive];
        if (!dev->irq_expected)
            continue;

        dev->irq_fired = 1;
        ata_pio_clear_status(dev);
    }

    IR_EOI(ATA_SECONDARY_IRQ);
}

static int32_t ide_read(void *impl, uint64_t lba, uint32_t sector_count, void *buffer)
{
    ide_device_t *dev = (const ide_device_t *)impl;

    uint8_t *buf8 = (uint8_t *)buffer;

    STORAGE_CONTROLLER_SET_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_READING);
    STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_ERROR);

    for (uint32_t i = 0; i < sector_count; i++)
    {
        if (ata_pio_read_sector(dev, lba + i, buf8 + i * dev->sector_size) != ATA_NO_ERROR)
        {
            STORAGE_CONTROLLER_SET_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_ERROR);
            STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_READING);
            return -1;
        }
    }

    STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_READING);

    return sector_count;
}

static int32_t ide_write(void *impl, uint64_t lba, uint32_t sector_count, const void *buffer)
{
    ide_device_t *dev = (const ide_device_t *)impl;

    const uint8_t *buf8 = (const uint8_t *)buffer;

    STORAGE_CONTROLLER_SET_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_WRITING);
    STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_ERROR);

    for (uint32_t i = 0; i < sector_count; i++)
    {
        if (ata_pio_write_sector(dev, lba + i, buf8 + i * dev->sector_size) != ATA_NO_ERROR)
        {
            STORAGE_CONTROLLER_SET_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_ERROR);
            STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_WRITING);
            return -1;
        }
    }

    STORAGE_CONTROLLER_CLEAR_FLAG(dev->controller_status, STORAGE_CONTROLLER_FLAG_BUSY | STORAGE_CONTROLLER_FLAG_WRITING);

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

static uint32_t ide_status(void *impl)
{
    const ide_device_t *dev = (const ide_device_t *)impl;
    return dev->controller_status;
}


static uint8_t ide_pci_probe(const pci_device_t *pci_dev)
{
    if (pci_dev->header.type0.common.class_code != 0x1 ||
        pci_dev->header.type0.common.subclass_code != 0x1)
        return 1;

    kernel_interrupts_add_request_handle(ATA_PRIMARY_IRQ, ata_primary_irq_handle);
    kernel_interrupts_add_request_handle(ATA_SECONDARY_IRQ, ata_secondary_irq_handle);

    for (uint8_t channel = 0; channel <= ATA_SECONDARY; channel++)
    {
        for (uint8_t drive = ATA_MASTER; drive <= ATA_SLAVE; drive++)
        {
            uint8_t dev_idx = (channel << 1) | drive;

            uint8_t code = ata_pio_init_drive(channel, drive, &ide_devices[dev_idx]);
            if (code != 0){
                continue;
            }

            const char *name = ide_devices[dev_idx].model;
            void *impl = (void *)&ide_devices[dev_idx];
            const storage_controller_ops_t *ops = &ide_ops;
            const uint32_t flags = 0;

            storage_manager_add_controller(&kernel_context->storage_manager, (storage_controller_t){ 
                name,
                impl,
                ops,
                flags,
                -1
            });
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