/// ---------------
/// @file ata-pio.c

#include "kernel/drivers/ata-pio.h"

#include <string.h>
#include <memory.h>

uint8_t ata_pio_init_drive(const pci_device_t *dev, uint8_t drive, ide_device_t *dev_out)
{
    if (!dev_out)
        return 1;

    uint8_t channel = ((dev->bus == ATA_PRIMARY) ? ATA_PRIMARY : ATA_SECONDARY);
    uint16_t io = ((channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);

    dev_out->channel = channel;
    dev_out->drive = drive;

    ata_pio_select_drive(channel, drive);

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
            memset((uint8_t *)(dev_out), 0, sizeof(ide_device_t));
            return 1;
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
            *(uint16_t *)(dev_out->identify_data + i * 2) = inports(io + ATA_REG_DATA);
        }

        for (int i = 0; i < ATA_IDE_MODEL_LENGTH - 1; i += 2)
        {
            dev_out->model[i] = dev_out->identify_data[ATA_IDENT_MODEL + i + 1];
            dev_out->model[i + 1] = dev_out->identify_data[ATA_IDENT_MODEL + i];
        }

        trim(dev_out->model);

        dev_out->signature = *((unsigned short *)(dev_out->identify_data + ATA_IDENT_DEVICETYPE));
        dev_out->command_sets = *((unsigned int *)(dev_out->identify_data + ATA_IDENT_COMMANDSETS));

        if (dev_out->command_sets & (1 << 26))
            dev_out->sector_size = *((unsigned int *)(dev_out->identify_data + ATA_IDENT_MAX_LBA_EXT));
        else
            dev_out->sector_size = *((unsigned int *)(dev_out->identify_data + ATA_IDENT_MAX_LBA));
    }
    else
    {
        memset((uint8_t *)(dev_out), 0, sizeof(ide_device_t));
        return 3;
    }

    return 0;
}

void ata_pio_select_drive(uint8_t channel, uint8_t drive)
{
    uint16_t io = ((channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);
    uint8_t hddevsel = (drive == ATA_MASTER) ? 0xA0 : 0xB0;

    outportb(io + ATA_REG_HDDEVSEL, hddevsel);
    ata_pio_io_wait(io);
}

uint8_t ata_pio_status_wait(int32_t io_base, int32_t timeout)
{
    uint8_t status;

    if (timeout > 0)
    {
        int32_t i = 0;
        while (
            (status = inportb(io_base + ATA_REG_STATUS)) & ATA_SR_BSY && (i < timeout))
            i++;
    }
    else
    {
        while ((status = inportb(io_base + ATA_REG_STATUS)) & ATA_SR_BSY)
            ;
    }
    return status;
}

void ata_pio_io_wait(int32_t io_base)
{
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
}

int32_t ata_pio_wait(int32_t io, uint8_t adv)
{
    uint8_t status = 0;

    ata_pio_io_wait(io);

    status = ata_pio_status_wait(io, -1);

    if (adv)
    {
        status = inportb(io + ATA_REG_STATUS);
        if (status & ATA_SR_ERR)
            return -1;
        if (status & ATA_SR_DF)
            return -2;
        if (!(status & ATA_SR_DRQ))
            return -3;
    }

    return 0;
}

int32_t ata_pio_read_sector(int32_t io_base, uint8_t drive, uint64_t lba, void *buf)
{
    uint16_t *buf16 = (uint16_t *)buf;

    outportb(io_base + ATA_REG_CONTROL, 0x02);
    ata_pio_io_wait(io_base);

    outportb(io_base + ATA_REG_HDDEVSEL, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));
    ata_pio_io_wait(io_base);

    outportb(io_base + ATA_REG_FEATURES, 0x00);
    outportb(io_base + ATA_REG_SECCOUNT0, 1);
    outportb(io_base + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outportb(io_base + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outportb(io_base + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));

    outportb(io_base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

    if (ata_pio_wait(io_base, 1))
        return -1;

    for (int i = 0; i < 256; i++)
        buf16[i] = inports(io_base + ATA_REG_DATA);

    ata_pio_wait(io_base, 0);

    return 0;
}

int32_t ata_pio_write_sector(int32_t io_base, uint8_t drive, uint64_t lba, const void *buf)
{
    const uint16_t *buf16 = (const uint16_t *)buf;

    outportb(io_base + ATA_REG_CONTROL, 0x02);
    ata_pio_io_wait(io_base);

    outportb(io_base + ATA_REG_HDDEVSEL, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));
    ata_pio_io_wait(io_base);

    outportb(io_base + ATA_REG_FEATURES, 0x00);
    outportb(io_base + ATA_REG_SECCOUNT0, 1);
    outportb(io_base + ATA_REG_LBA0, (uint8_t)(lba & 0xFF));
    outportb(io_base + ATA_REG_LBA1, (uint8_t)((lba >> 8) & 0xFF));
    outportb(io_base + ATA_REG_LBA2, (uint8_t)((lba >> 16) & 0xFF));

    outportb(io_base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

    if (ata_pio_wait(io_base, 1))
        return -1;

    for (int i = 0; i < 256; i++)
    {
        outports(io_base + ATA_REG_DATA, buf16[i]);
    }
    outportb(io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);

    ata_pio_wait(io_base, 0);

    return 0;
}