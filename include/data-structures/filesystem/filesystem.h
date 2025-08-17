/// ------------
/// filesystem.h
/// @brief This file

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <data-structures/filesystem/fat16.h>
#include <data-structures/disk-manager/disk-manager.h>

#define MOUNT_MAX_COUNT 512

typedef struct filesystem_mount_s
{
    fat16_boot_record_t boot_table;
    fat_entry_t root_entry;
    uint32_t start_lba;
    uint8_t *fat_table_ptr;
    
    uint32_t current_block;
    uint32_t current_cluster;

    struct filesystem_s *filesystem;
    disk_device_t *disk;
} filesystem_mount_t;

typedef struct filesystem_s
{
    disk_manager_t *disk_manager;
    filesystem_mount_t mounts[MOUNT_MAX_COUNT];
    uint16_t mount_count;
} filesystem_t;

typedef struct file_info_s
{
    char name[256];
    uint8_t attributes;
    uint32_t size;

    uint32_t created;
    uint32_t modified;
    uint32_t accessed;
} file_info_t;

#define ENTRIES_PER_BLOCK (512 / sizeof(fat_entry_t))

#endif