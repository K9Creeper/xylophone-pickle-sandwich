/// --------------
/// disk-manager.h
/// @brief This file defines

#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <stdbool.h>
#include <data-structures/drivers/ata.h>

#define DISK_DEVICE_MAX_COUNT 128

#define MBR_PARTITION_1 0x01BE
#define MBR_PARTITION_2 0x01CE
#define MBR_PARTITION_3 0x01DE
#define MBR_PARTITION_4 0x01EE

typedef int32_t (*disk_manager_read_t)(char *buffer, uint32_t from, uint32_t size);
typedef int32_t (*disk_manager_write_t)(char *buffer, uint32_t from, uint32_t size);
typedef void (*disk_manager_set_disk_t)(ide_device_t* dev);

typedef struct disk_device_s
{
    disk_manager_read_t read;
    disk_manager_write_t write;
    disk_manager_set_disk_t set_disk;
    
    ide_device_t* dev;
} disk_device_t;

typedef struct disk_manager_s
{
    disk_device_t disk_devices[DISK_DEVICE_MAX_COUNT];
    int8_t disk_count;
    int8_t attached_disk;
} disk_manager_t;

typedef struct partition_table_entry_s{
    uint8_t boot_indicator;
    uint8_t start_head;

    uint8_t start_sector : 6;
    uint16_t start_cylinder : 10;

    uint8_t system_id;  
    uint8_t end_head;

    uint8_t end_sector : 6;
    uint16_t end_cylinder : 10;

    uint32_t start_lba;
    uint32_t total_sectors;
}__attribute__((packed)) partition_table_entry_t;

#endif