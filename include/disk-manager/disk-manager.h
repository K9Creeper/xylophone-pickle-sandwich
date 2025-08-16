/// --------------
/// disk-manager.h
/// @brief This file defines

#ifndef _DISK_MANAGER_H
#define _DISK_MANAGER_H

#include <data-structures/disk-manager/disk-manager.h>

extern void disk_manager_init(disk_manager_t* disk_manager);

extern disk_device_t* disk_manager_get_devices(disk_manager_t* disk_manager, uint8_t* count);

extern void disk_manager_add_device(disk_manager_t* disk_manager, ide_device_t* dev, disk_manager_read_t read, disk_manager_write_t write, disk_manager_set_disk_t set_disk);
extern void disk_manager_attach(disk_manager_t *disk_manager, ide_device_t *dev);

extern const char *disk_manager_get_disk_name(disk_manager_t* disk_manager);
extern int disk_manager_get_sector_count(disk_manager_t* disk_manager);
extern bool disk_manager_is_attached(disk_manager_t *disk_manager);

extern int disk_manager_write_block(disk_manager_t *disk_manager, disk_device_t* dev, void* buffer, int block);
extern int disk_manager_write_block_offset(disk_manager_t *disk_manager, disk_device_t* dev, void* buffer, int size, int offset, int block);
extern int disk_manager_read_block(disk_manager_t *disk_manager, disk_device_t* dev, void* buffer, int block);
extern int disk_manager_read_block_offset(disk_manager_t *disk_manager, disk_device_t* dev, void* buffer, int size, int offset, int block);

#endif