/// --------------
/// disk-manager.c
/// @brief This file defines

#include <disk-manager/disk-manager.h>

#include <stddef.h>
#include <memory.h>

void disk_manager_init(disk_manager_t *disk_manager)
{
    if (disk_manager == NULL)
        return;
    disk_manager->attached_disk = -1;
    disk_manager->disk_count = 0;
    memset((uint8_t *)disk_manager->disk_devices, 0, sizeof(disk_device_t) * DISK_DEVICE_MAX_COUNT);
}

disk_device_t *disk_manager_get_devices(disk_manager_t *disk_manager, uint8_t *count)
{
    if (disk_manager == NULL)
        return NULL;
    if (count != NULL)
        *count = disk_manager->disk_count;
    return disk_manager->disk_devices;
}

void disk_manager_add_device(disk_manager_t *disk_manager, ide_device_t *dev, disk_manager_read_t read, disk_manager_write_t write, disk_manager_set_disk_t set_disk)
{
    if (disk_manager == NULL || disk_manager->disk_count >= DISK_DEVICE_MAX_COUNT)
        return;

    disk_device_t *ddevice = &disk_manager->disk_devices[disk_manager->disk_count++];
    ddevice->dev = dev;
    ddevice->read = read;
    ddevice->write = write;
    ddevice->set_disk = set_disk;
}

void disk_manager_attach(disk_manager_t *disk_manager, ide_device_t *dev)
{
    if (disk_manager == NULL || dev == NULL)
        return;

    for (int i = 0; i < disk_manager->disk_count; i++)
    {
        disk_device_t *ddevice = &disk_manager->disk_devices[i];
        if (ddevice->dev != dev)
            continue;

        if (ddevice->set_disk != NULL)
            ddevice->set_disk(ddevice->dev);
        disk_manager->attached_disk = i;
        break;
    }
}

const char *disk_manager_get_disk_name(disk_manager_t *disk_manager)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || disk_manager->disk_devices[disk_manager->attached_disk].dev == NULL)
        return NULL;

    return disk_manager->disk_devices[disk_manager->attached_disk].dev->model;
}

int disk_manager_get_sector_count(disk_manager_t *disk_manager)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || disk_manager->disk_devices[disk_manager->attached_disk].dev == NULL)
        return -1;

    return disk_manager->disk_devices[disk_manager->attached_disk].dev->size / 512;
}

bool disk_manager_is_attached(disk_manager_t *disk_manager)
{
    if (disk_manager == NULL)
        return false;

    return disk_manager->attached_disk >= 0;
}

int disk_manager_write_block(disk_manager_t *disk_manager, disk_device_t *dev, void *buffer, int block)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || buffer == NULL || dev == NULL)
        return -1;

    disk_device_t *disk_device = &disk_manager->disk_devices[disk_manager->attached_disk];

    if (dev->write == NULL  || dev->set_disk == NULL || dev->dev == NULL)
        return -1;

    if (disk_device != dev)
        disk_manager_attach(disk_manager, dev->dev);

    return dev->write(buffer, block, 1);
}

int disk_manager_write_block_offset(disk_manager_t *disk_manager, disk_device_t *dev, void *buffer, int size, int offset, int block)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || buffer == NULL || dev == NULL)
        return -1;

    char buf[512];

    disk_device_t *disk_device = &disk_manager->disk_devices[disk_manager->attached_disk];
    if (dev->read == NULL || dev->write == NULL  || dev->set_disk == NULL || dev->dev == NULL)
        return -1;

    if (disk_device != dev)
        disk_manager_attach(disk_manager, dev->dev);

    if (disk_manager_read_block(disk_manager, dev, buf, block) < 0)
        return -1;

    memcpy(&buf[offset], buffer, size);

    return disk_manager_write_block(disk_manager, dev, buf, block);
}

int disk_manager_read_block(disk_manager_t *disk_manager, disk_device_t *dev, void *buffer, int block)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || buffer == NULL || dev == NULL)
        return -1;

    disk_device_t *disk_device = &disk_manager->disk_devices[disk_manager->attached_disk];

    if (dev->read == NULL || dev->set_disk == NULL || dev->dev == NULL)
        return -1;

    if (disk_device != dev)
        disk_manager_attach(disk_manager, dev->dev);

    return dev->read(buffer, block, 1);
}

int disk_manager_read_block_offset(disk_manager_t *disk_manager, disk_device_t *dev, void *buffer, int size, int offset, int block)
{
    if (disk_manager == NULL || disk_manager->attached_disk < 0 || disk_manager->attached_disk >= disk_manager->disk_count || buffer == NULL || dev == NULL)
        return -1;

    char buf[512];

    if (disk_manager_read_block(disk_manager, dev, buf, block) < 0)
        return -1;

    memcpy(buffer, &buf[offset], size);

    return size;
}