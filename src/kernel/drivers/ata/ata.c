/// -----
/// ata.c
/// @brief This file

#include "ata.h"

#include <stddef.h>
#include <memory.h>
#include <string.h>

#include <kernel/util.h>
#include "../../interrupts/interrupt-request.h"

#include <data-structures/kernel/kernel-context.h>
extern kernel_context_t *kernel_context;

static ide_device_t ide_devices[4];

static int ata_status_wait(int io_base, int timeout)
{
    int status;

    if (timeout > 0)
    {
        int i = 0;
        while ((status = inportb(io_base + ATA_REG_STATUS)) & ATA_SR_BSY && (i < timeout))
            i++;
    }
    else
    {
        while ((status = inportb(io_base + ATA_REG_STATUS)) & ATA_SR_BSY)
            ;
    }
    return status;
}

static void ata_io_wait(int io_base)
{
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
}

static int ata_wait(int io, int adv)
{
    uint8_t status = 0;

    ata_io_wait(io);

    status = ata_status_wait(io, -1);

    if (adv)
    {
        status = inportb(io + ATA_REG_STATUS);
        if (status & ATA_SR_ERR)
            return 1;
        if (status & ATA_SR_DF)
            return 1;
        if (!(status & ATA_SR_DRQ))
            return 1;
    }

    return 0;
}

static int ata_read_sector(char *buf, int lba)
{
    uint16_t io = ATA_PRIMARY_IO;

    uint8_t cmd = 0xE0;
    int errors = 0;

ata_read_sector_try_again:
    outportb(io + ATA_REG_CONTROL, 0x02);

    ata_wait(io, 0);

    outportb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
    outportb(io + ATA_REG_FEATURES, 0x00);
    outportb(io + ATA_REG_SECCOUNT0, 1);
    outportb(io + ATA_REG_LBA0, (uint8_t)(lba));
    outportb(io + ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outportb(io + ATA_REG_LBA2, (uint8_t)(lba >> 16));
    outportb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    if (ata_wait(io, 1))
    {
        errors++;
        if (errors > 4)
            return -1;

        goto ata_read_sector_try_again;
    }

    for (int i = 0; i < 256; i++)
    {
        uint16_t d = inports(io + ATA_REG_DATA);
        *(uint16_t *)(buf + i * 2) = d;
    }

    ata_wait(io, 0);
    return 0;
}

static int ata_write_sector(uint16_t *buf, int lba)
{
    uint16_t io = ATA_PRIMARY_IO;

    uint8_t cmd = 0xE0;

    outportb(io + ATA_REG_CONTROL, 0x02);

    ata_wait(io, 0);

    outportb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
    ata_wait(io, 0);
    outportb(io + ATA_REG_FEATURES, 0x00);
    outportb(io + ATA_REG_SECCOUNT0, 1);
    outportb(io + ATA_REG_LBA0, (uint8_t)(lba));
    outportb(io + ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outportb(io + ATA_REG_LBA2, (uint8_t)(lba >> 16));
    outportb(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    ata_wait(io, 0);

    for (int i = 0; i < 256; i++)
    {
        outports(io + ATA_REG_DATA, buf[i]);
        __asm__ volatile("nop");
    }
    outportb(io + 0x07, ATA_CMD_CACHE_FLUSH);

    ata_wait(io, 0);

    return 0;
}

static int ata_write(char *buf, uint32_t from, uint32_t count)
{
    unsigned long pos = from;

    DISABLE_INTERRUPTS();
    for (uint32_t i = 0; i < count; i++)
    {
        ata_write_sector((uint16_t *)buf, pos + i);
        buf += 512;
        for (int j = 0; j < 1000; j++)
            ;
    }
    ENABLE_INTERRUPTS();
    return count;
}

static int ata_read(char *buf, uint32_t from, uint32_t numsects)
{
    unsigned long pos = from;
    int rc = 0;

    DISABLE_INTERRUPTS();

    for (uint32_t i = 0; i < numsects; i++)
    {
        rc = ata_read_sector((char *)buf, pos + i);
        if (rc == -1)
        {
            ENABLE_INTERRUPTS();
            return -1;
        }
        buf += 512;
    }

    ENABLE_INTERRUPTS();

    return rc;
}

static void ata_select_drive(ide_device_t *dev)
{
    uint16_t io = ((dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);

    uint8_t hddevsel = 0xA0;
    if (dev->drive != ATA_MASTER)
        hddevsel = 0xB0;

    outportb(io + ATA_REG_HDDEVSEL, hddevsel);
    ata_io_wait(io);
}

#include <stdio.h>
void ata_init(pci_device_t *dev)
{
    uint8_t channel = ((dev->bus == ATA_PRIMARY) ? ATA_PRIMARY : ATA_SECONDARY);
    uint16_t io = ((channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);

    for (uint8_t drive = ATA_MASTER; drive <= ATA_SLAVE; drive++)
    {
        const uint8_t index = channel * 2 + drive;
        ide_devices[index].channel = channel;
        ide_devices[index].drive = drive;

        ata_select_drive(&ide_devices[index]);

        outportb(io + ATA_REG_SECCOUNT0, 0);
        outportb(io + ATA_REG_LBA0, 0);
        outportb(io + ATA_REG_LBA1, 0);
        outportb(io + ATA_REG_LBA2, 0);

        outportb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

        uint8_t status = inportb(io + ATA_REG_STATUS);
        if (status)
        {
            uint8_t status = inportb(io + ATA_REG_STATUS);
            if (status == 0 || status == 0xFF)
            {
                memset((uint8_t *)(&ide_devices[index]), 0, sizeof(ide_device_t));
                continue;
            }

            while (1)
            {
                status = inportb(io + ATA_REG_STATUS);
                if (status & ATA_SR_ERR)
                    break;
                if (status & ATA_SR_DRQ)
                    break;
            }

            for (int i = 0; i < 256; i++)
            {
                *(uint16_t *)(ide_devices[index].driver_data + i * 2) = inports(io + ATA_REG_DATA);
            }

            for (int i = 0; i < ATA_IDE_MODEL_LENGTH - 1; i += 2)
            {
                ide_devices[index].model[i] = ide_devices[index].driver_data[ATA_IDENT_MODEL + i + 1];
                ide_devices[index].model[i + 1] = ide_devices[index].driver_data[ATA_IDENT_MODEL + i];
            }

            trim(ide_devices[index].model);

            ide_devices[index].signature = *((unsigned short *)(ide_devices[index].driver_data + ATA_IDENT_DEVICETYPE));
            ide_devices[index].command_sets = *((unsigned int *)(ide_devices[index].driver_data + ATA_IDENT_COMMANDSETS));

            if (ide_devices[index].command_sets & (1 << 26))
                ide_devices[index].size = *((unsigned int *)(ide_devices[index].driver_data + ATA_IDENT_MAX_LBA_EXT));
            else
                ide_devices[index].size = *((unsigned int *)(ide_devices[index].driver_data + ATA_IDENT_MAX_LBA));

            disk_manager_add_device(&kernel_context->disk_manager, &ide_devices[index], (disk_manager_read_t)ata_read, (disk_manager_write_t)ata_write, (disk_manager_set_disk_t)ata_select_drive);
            break;
        }
        else
        {
            memset((uint8_t *)(&ide_devices[index]), 0, sizeof(ide_device_t));
        }
    }
}