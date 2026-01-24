/// ---------------
/// @file ata-pio.c

#include <kernel/drivers/ata-pio.h>

#include <string.h>
#include <memory.h>

#include <kernel/util.h>

static inline ata_error_t ata_pio_check_error(uint16_t io_base)
{
    uint8_t status = inportb(io_base + ATA_REG_STATUS);

    if (status & ATA_SR_DF) return ATA_ERR_DRIVE_FAULT;
    if (status & ATA_SR_ERR)
    {
        uint8_t err = inportb(io_base + ATA_REG_ERROR);

        if (err & 0x80) return ATA_ERR_BAD_BLOCK;
        if (err & 0x40) return ATA_ERR_UNCORRECTABLE;
        if (err & 0x20) return ATA_ERR_MEDIA_CHANGED;
        if (err & 0x10) return ATA_ERR_IDNF;
        if (err & 0x08) return ATA_ERR_MCR;
        if (err & 0x04) return ATA_ERR_CMD_ABORTED;
        if (err & 0x02) return ATA_ERR_TK0NF;
        if (err & 0x01) return ATA_ERR_AMNF;

        return ATA_ERR_UNKNOWN;
    }

    return ATA_NO_ERROR;
}

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
        status = inportb(io + ATA_REG_STATUS);
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

        if (status & ATA_SR_ERR)
        {
            memset((uint8_t *)dev_out, 0, sizeof(ide_device_t));
            return 1;
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
        {
            dev_out->sector_count = *((uint64_t *)(dev_out->identify_data + ATA_IDENT_MAX_LBA_EXT));
        }
        else
        {
            dev_out->sector_count = *((uint32_t *)(dev_out->identify_data + ATA_IDENT_MAX_LBA));
        }

        dev_out->sector_size = 512;

        dev_out->irq_expected = 0;
        dev_out->irq_fired = 0;
    }
    else
    {
        memset((uint8_t *)(dev_out), 0, sizeof(ide_device_t));
        return 3;
    }

    return 0;
}

static inline void ata_pio_io_wait(int32_t io_base)
{
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
    inportb(io_base + ATA_REG_ALTSTATUS);
}

static inline ata_error_t ata_wait_ready_and_drq(uint16_t io_base)
{
    uint8_t status;
    int32_t timeout = 1000000;

    do {
        status = inportb(io_base + ATA_REG_STATUS);
        if (--timeout == 0) return ATA_ERR_BSY_TIMEOUT;
    } while (status & ATA_SR_BSY);

    if (!(status & ATA_SR_DRDY)) return ATA_ERR_DRQ_TIMEOUT;

    return ATA_NO_ERROR;
}

static inline void ata_wait_irq(ide_device_t *dev) {
    dev->irq_expected = 1;
    dev->irq_fired = 0;

    while (!dev->irq_fired) {
        __asm__ volatile ("nop");
    }

    dev->irq_expected = 0;
    dev->irq_fired = 0;
}

void ata_pio_clear_status(ide_device_t *dev){
    uint16_t io = ((dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);
    inportb(io + ATA_REG_STATUS);
}

ata_error_t ata_pio_select_drive(uint8_t channel, uint8_t drive)
{
    uint16_t io = ((channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO);
    uint8_t hddevsel = (drive == ATA_MASTER) ? 0xA0 : 0xB0;

    outportb(io + ATA_REG_HDDEVSEL, hddevsel);
    ata_pio_io_wait(io);
}

ata_error_t ata_pio_read_sector(ide_device_t *dev, uint64_t lba, void *buf)
{
    if (dev->command_sets & (1 << 26))
        return ATA_ERR_48BIT_UNSUPPORTED;

    const uint16_t io_base = (dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    const uint8_t drive = dev->drive;

    ata_error_t err = ata_wait_ready_and_drq(io_base);
    if (err != ATA_NO_ERROR) return err;

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

    ata_wait_irq(dev);

    for (int i = 0; i < 256; i++)
        buf16[i] = inports(io_base + ATA_REG_DATA);

    return ata_pio_check_error(io_base);
}

ata_error_t ata_pio_write_sector(ide_device_t *dev, uint64_t lba, const void *buf)
{
    if (dev->command_sets & (1 << 26))
        return ATA_ERR_48BIT_UNSUPPORTED;

    const uint16_t io_base = (dev->channel == ATA_PRIMARY) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    const uint8_t drive = dev->drive;

    ata_error_t err = ata_wait_ready_and_drq(io_base);
    if (err != ATA_NO_ERROR) return err;

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

    ata_wait_irq(dev);

    for (int i = 0; i < 256; i++)
        outports(io_base + ATA_REG_DATA, buf16[i]);

    if (dev->command_sets & ATA_CMD_CACHE_FLUSH)
    {
        outportb(io_base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
        ata_wait_irq(dev);
    }

    return ata_pio_check_error(io_base);
}