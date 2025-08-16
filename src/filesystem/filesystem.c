/// ------------
/// filesystem.c
/// @brief This file

#include <filesystem/filesystem.h>
#include <disk-manager/disk-manager.h>
#include <kernel/stdlib.h>
#include <kernel/util.h>

#include <scheduling/task.h>

#include <stddef.h>

#include <memory.h>
#include <string.h>
#include <math.h>

#include <syscall.h>

#include <stdio.h>

static inline uint32_t get_fat_start_block(filesystem_mount_t *mount)
{
    return mount->start_lba + mount->boot_table.boot_record.reserved_blocks;
}

static inline uint32_t get_root_directory_start_block(filesystem_mount_t *mount)
{
    return mount->start_lba + mount->boot_table.boot_record.reserved_blocks + (mount->boot_table.boot_record.num_FATs * mount->boot_table.boot_record.fat_blocks);
}

static inline uint32_t get_data_start_block(filesystem_mount_t *mount)
{
    uint32_t root_dir_sectors = (mount->boot_table.boot_record.root_dir_entries * 32 + 511) / 512;
    return mount->start_lba + mount->boot_table.boot_record.reserved_blocks + (mount->boot_table.boot_record.num_FATs * mount->boot_table.boot_record.fat_blocks) + root_dir_sectors;
}

static inline uint32_t get_data_cluster_lba(filesystem_mount_t *mount, uint32_t cluster)
{
    return mount->start_lba + mount->boot_table.boot_record.reserved_blocks + (mount->boot_table.boot_record.num_FATs * mount->boot_table.boot_record.fat_blocks) + (cluster - 2) * mount->boot_table.boot_record.sectors_per_cluster;
}

static char *sstrtok(char *str, const char *delim);

static void fat16_to_upper(char *str);

static int fat16_load(filesystem_mount_t *mount);

static int fat16_read_entry(filesystem_mount_t *mount, uint32_t block, uint32_t index, fat_entry_t *out);
static int fat16_list_entries(filesystem_mount_t *mount, fat_entry_t *out_entries, uint32_t max_entries);
static int fat16_locate_entry(filesystem_mount_t *mount, const char *name, const char *ext, fat_entry_t *out);
static int fat16_change_directory(filesystem_mount_t *mount, const char *name);

static uint32_t fat16_read_data(filesystem_mount_t *mount, fat_entry_t *entry, void *buffer, uint32_t size, uint32_t offset);

void filesystem_init(filesystem_t *filesystem, disk_manager_t *disk_manager)
{
    if (filesystem == NULL || disk_manager == NULL)
        return;

    filesystem->disk_manager = disk_manager;
    filesystem->mount_count = 0;

    for (int i = 0; i < disk_manager->disk_count; i++)
    {
        // Both arrays (diskmanager and filesystem) are (supposed to be) parallel..
        disk_device_t *disk = &disk_manager->disk_devices[i];
        filesystem_mount_t *mount = &filesystem->mounts[filesystem->mount_count];
        mount->filesystem = filesystem;
        mount->disk = disk;

        if (filesystem_mount(filesystem, filesystem->mount_count) != NULL)
        {
            filesystem->mount_count++;
        }
    }

    if (filesystem->mount_count > 0)
    {
        filesystem_mount_t *mount = filesystem_mount(filesystem, 0);
    }
}

filesystem_mount_t *filesystem_mount(filesystem_t *filesystem, int drive_num)
{
    DISABLE_INTERRUPTS();
    if (filesystem == NULL || filesystem->disk_manager == NULL)
    {
        ENABLE_INTERRUPTS();
        return NULL;
    }
    if (drive_num < 0)
    {
        ENABLE_INTERRUPTS();
        return NULL;
    }
    if (drive_num >= filesystem->disk_manager->disk_count)
    {
        ENABLE_INTERRUPTS();
        return NULL;
    }
    if (filesystem->mount_count != 0 && drive_num >= filesystem->mount_count)
    {
        ENABLE_INTERRUPTS();
        return NULL;
    }

    filesystem_mount_t *mount = &filesystem->mounts[drive_num];

    disk_manager_attach(filesystem->disk_manager, mount->disk->dev);

    if (fat16_load(mount) < 0)
    {
        ENABLE_INTERRUPTS();
        return NULL;
    }

    ENABLE_INTERRUPTS();
    return mount;
}

bool filesystem_get_entry(filesystem_t *filesystem, const char *path, fat_entry_t *out)
{
    if (!filesystem || !path || !out)
        return false;

    DISABLE_INTERRUPTS();

    int drive_num = 0;
    const char *relative_path = path;
    static char full_path[TASK_MAX_FILESYSTEM_PATH_LENGTH];

    if (path[1] == ':' && path[2] == '/')
    {
        drive_num = path[0] - '0';
        if (drive_num < 0 || drive_num >= filesystem->mount_count)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        if (filesystem_mount(filesystem, drive_num) == NULL)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        relative_path = path + 3;
    }
    else
    {
        char current_task_path[512];
        get_task_directory(current_task_path, sizeof(current_task_path));

        int len = 0;
        while (current_task_path[len] != '\0' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len] = current_task_path[len];
            len++;
        }

        if (len > 0 && full_path[len - 1] != '/' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len++] = '/';
        }

        int i = 0;
        while (path[i] != '\0' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len++] = path[i++];
        }

        full_path[len] = '\0';
        relative_path = full_path;
    }

    filesystem_mount_t *mount = filesystem_mount(filesystem, drive_num);
    if (!mount)
    {
        ENABLE_INTERRUPTS();
        return false;
    }

    if (relative_path[1] == ':' && relative_path[2] == '/')
    {
        relative_path += 3;
    }

    fat_entry_t current_entry;

    if (fat16_read_entry(mount, mount->current_block, 0, &current_entry) < 0)
    {
        ENABLE_INTERRUPTS();
        return false;
    }

    char *segment = sstrtok((char *)relative_path, "/");
    char *next_segment = sstrtok(NULL, "/");

    while (segment != NULL)
    {
        char name[9] = {0};
        char ext[4] = {0};

        char *dot = strchr(segment, '.');
        if (dot != NULL)
        {
            int namelen = dot - segment;
            if (namelen > 8)
                namelen = 8;
            memcpy(name, segment, namelen);

            int extlen = strlen(dot + 1);
            if (extlen > 3)
                extlen = 3;
            memcpy(ext, dot + 1, extlen);
        }
        else
        {
            int namelen = strlen(segment);
            if (namelen > 8)
                namelen = 8;
            memcpy(name, segment, namelen);

            memset(ext, ' ', 3);
        }

        fat16_to_upper(name);
        fat16_to_upper(ext);
        fat16_to_upper(segment);

        if (fat16_locate_entry(mount, name, ext, &current_entry) < 0)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        if ((current_entry.attributes & FAT_DIRECTORY) && next_segment != NULL)
        {
            if (fat16_change_directory(mount, segment) < 0)
            {
                ENABLE_INTERRUPTS();
                return false;
            }
        }

        segment = next_segment;
        next_segment = sstrtok(NULL, "/");
    }

    *out = current_entry;
    ENABLE_INTERRUPTS();
    return true;
}

extern bool filesystem_get_entries(filesystem_t *filesystem, const char *path, fat_entry_t *out_entries, uint32_t max_entries)
{
    if (!filesystem || !path || !out_entries)
        return false;

    DISABLE_INTERRUPTS();

    int drive_num = 0;
    const char *relative_path = path;
    static char full_path[TASK_MAX_FILESYSTEM_PATH_LENGTH];

    if (path[1] == ':' && path[2] == '/')
    {
        drive_num = path[0] - '0';
        if (drive_num < 0 || drive_num >= filesystem->mount_count)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        if (filesystem_mount(filesystem, drive_num) == NULL)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        relative_path = path + 3;
    }
    else
    {
        char current_task_path[512];
        get_task_directory(current_task_path, sizeof(current_task_path));

        int len = 0;
        while (current_task_path[len] != '\0' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len] = current_task_path[len];
            len++;
        }

        if (len > 0 && full_path[len - 1] != '/' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len++] = '/';
        }

        int i = 0;
        while (path[i] != '\0' && len < TASK_MAX_FILESYSTEM_PATH_LENGTH - 1)
        {
            full_path[len++] = path[i++];
        }

        full_path[len] = '\0';
        relative_path = full_path;
    }

    filesystem_mount_t *mount = filesystem_mount(filesystem, drive_num);
    if (!mount)
    {
        ENABLE_INTERRUPTS();
        return false;
    }

    if (relative_path[1] == ':' && relative_path[2] == '/')
    {
        relative_path += 3;
    }

    fat_entry_t current_entry;

    if (fat16_read_entry(mount, mount->current_block, 0, &current_entry) < 0)
    {
        ENABLE_INTERRUPTS();
        return false;
    }

    char *segment = sstrtok((char *)relative_path, "/");
    char *next_segment = sstrtok(NULL, "/");

    while (segment != NULL)
    {
        char name[9] = {0};
        char ext[4] = {0};

        char *dot = strchr(segment, '.');
        if (dot != NULL)
        {
            int namelen = dot - segment;
            if (namelen > 8)
                namelen = 8;
            memcpy(name, segment, namelen);

            int extlen = strlen(dot + 1);
            if (extlen > 3)
                extlen = 3;
            memcpy(ext, dot + 1, extlen);
        }
        else
        {
            int namelen = strlen(segment);
            if (namelen > 8)
                namelen = 8;
            memcpy(name, segment, namelen);

            memset(ext, ' ', 3);
        }

        fat16_to_upper(name);
        fat16_to_upper(ext);
        fat16_to_upper(segment);

        if (fat16_locate_entry(mount, name, ext, &current_entry) < 0)
        {
            ENABLE_INTERRUPTS();
            return false;
        }

        if ((current_entry.attributes & FAT_DIRECTORY))
        {
            if (fat16_change_directory(mount, segment) < 0)
            {
                ENABLE_INTERRUPTS();
                return false;
            }
        }

        segment = next_segment;
        next_segment = sstrtok(NULL, "/");
    }

    if (fat16_list_entries(mount, out_entries, max_entries) < 0)
    {
        ENABLE_INTERRUPTS();
        return false;
    }

    ENABLE_INTERRUPTS();
    return true;
}

uint32_t fat16_read_data(filesystem_mount_t *mount, fat_entry_t *entry, void *buffer, uint32_t size, uint32_t offset)
{
    if (mount == NULL || mount->fat_table_ptr == NULL)
        return 0;

    if (buffer == NULL || offset >= entry->file_size)
        return 0;

    if (offset + size > entry->file_size)
        size = entry->file_size - offset;

    uint16_t cluster_size = mount->boot_table.boot_record.sectors_per_cluster * 512;
    uint16_t cluster_index = offset / cluster_size;
    uint16_t in_cluster_offset = offset % cluster_size;

    uint16_t cluster = entry->cluster_num_low;

    for (uint16_t i = 0; i < cluster_index; i++)
    {
        uint16_t next = ((uint16_t *)mount->fat_table_ptr)[cluster];
        if (next >= 0xFFF8)
            return 0;
        cluster = next;
    }

    uint8_t *buf_ptr = (uint8_t *)buffer;
    uint32_t bytes_remaining = size;

    while (bytes_remaining > 0)
    {
        uint32_t lba = get_data_cluster_lba(mount, cluster);

        uint32_t to_read = cluster_size - in_cluster_offset;
        if (to_read > bytes_remaining)
            to_read = bytes_remaining;

        if (disk_manager_read_block_offset(mount->filesystem->disk_manager, mount->disk, buf_ptr, to_read, in_cluster_offset, lba) < 0)
            return 0;

        buf_ptr += to_read;
        bytes_remaining -= to_read;
        in_cluster_offset = 0;

        uint16_t next = ((uint16_t *)mount->fat_table_ptr)[cluster];
        if (next >= 0xFFF8)
            break;
        cluster = next;
    }

    return size - bytes_remaining;
}

int fat16_change_directory(filesystem_mount_t *mount, const char *name)
{
    if (!mount)
        return -1;

    fat_entry_t entry;
    int find_result = fat16_locate_entry(mount, name, "   ", &entry);
    if (find_result < 0 || !(entry.attributes & FAT_DIRECTORY))
        return -2;

    mount->current_block = get_data_start_block(mount) + (entry.cluster_num_low - 2) * mount->boot_table.boot_record.sectors_per_cluster;
    return 0;
}

int fat16_list_entries(filesystem_mount_t *mount, fat_entry_t *out_entries, uint32_t max_entries)
{
    if (mount->filesystem == NULL || mount->filesystem->disk_manager == NULL)
        return -1;

    for (int i = 0; i < imin(mount->boot_table.boot_record.root_dir_entries, max_entries); i++)
    {
        fat_entry_t entry = {0};

        fat16_read_entry(mount, mount->current_block, i, &entry);

        if ((entry.attributes & FAT_LFN) == FAT_LFN)
            continue;
        if (entry.filename[0] == 0x00)
            continue;
        if ((uint8_t)entry.filename[0] == 0xE5)
            continue;

        out_entries[i] = entry;
    }

    return 0;
}

int fat16_locate_entry(filesystem_mount_t *mount, const char *name, const char *ext, fat_entry_t *out)
{
    if (mount->filesystem == NULL || mount->filesystem->disk_manager == NULL)
        return -1;

    for (int i = 0; i < mount->boot_table.boot_record.root_dir_entries; i++)
    {
        fat_entry_t entry = {0};

        fat16_read_entry(mount, mount->current_block, i, &entry);

        if ((entry.attributes & FAT_LFN) == FAT_LFN)
            continue;
        if (entry.filename[0] == 0x00)
            continue;
        if ((uint8_t)entry.filename[0] == 0xE5)
            continue;

        if (memcmp(entry.name, name, strlen(name)) == 0 && memcmp(entry.extension, ext, 3) == 0)
        {
            if (out)
            {
                *out = entry;
            }

            return i;
        }
    }
    return -1;
}

int fat16_read_entry(filesystem_mount_t *mount, uint32_t block, uint32_t index, fat_entry_t *out)
{
    if (mount->filesystem == NULL || mount->filesystem->disk_manager == NULL)
        return -1;

    if (index >= ENTRIES_PER_BLOCK)
        return -1;

    if (block == get_root_directory_start_block(mount) && index == 0)
    {
        if (out != NULL)
            memcpy((uint8_t *)out, (uint8_t *)(&mount->root_entry), sizeof(fat_entry_t));
        return 0;
    }

    uint8_t buffer[512];
    if (disk_manager_read_block(mount->filesystem->disk_manager, mount->disk, buffer, block))
    {
        return -2;
    }

    fat_entry_t *dir_entry = (fat_entry_t *)((uint32_t)buffer + (index * sizeof(fat_entry_t)));

    if (out != NULL)
        memcpy((uint8_t *)out, (uint8_t *)dir_entry, sizeof(fat_entry_t));

    return 0;
}

int fat16_load(filesystem_mount_t *mount)
{
    if (mount->filesystem == NULL || mount->filesystem->disk_manager == NULL)
        return -1;

    if (!disk_manager_is_attached(mount->filesystem->disk_manager))
        return -1;

    if (mount == NULL)
        return -1;

    if (mount->fat_table_ptr != NULL)
        return 0;

    uint8_t mbr[512];
    int32_t read = disk_manager_read_block(mount->filesystem->disk_manager, mount->disk, mbr, 0);
    if (read)
        return -2;

    if (mbr[510] != 0x55 || mbr[511] != 0xAA)
        return -3;

    mount->start_lba = ((partition_table_entry_t *)(&mbr[MBR_PARTITION_1]))->start_lba;

    read = disk_manager_read_block(mount->filesystem->disk_manager, mount->disk, (void *)&mount->boot_table, mount->start_lba);

    if (read)
        return -4;

    uint32_t fat_table_size = mount->boot_table.boot_record.fat_blocks * 512;
    uint8_t *fat_table = mount->fat_table_ptr = kernel_calloc(1, fat_table_size);

    if (mount->fat_table_ptr == NULL)
        return -5;

    for (uint16_t block = 0; block < mount->boot_table.boot_record.fat_blocks; block++)
    {
        disk_manager_read_block(mount->filesystem->disk_manager, mount->disk, (fat_table + (block * 512)), get_fat_start_block(mount) + block);
    }

    mount->current_block = get_root_directory_start_block(mount);

    return 0;
}

static void fat16_to_upper(char *str)
{
    if (!str)
        return;

    while (*str)
    {
        if (*str >= 'a' && *str <= 'z')
        {
            *str = *str - ('a' - 'A');
        }
        str++;
    }
}

static char *sstrtok(char *str, const char *delim)
{
    static char local_buffer[128] = {0};
    static char *current = NULL;

    if (str != NULL)
    {
        int len = strlen(str);
        memset(local_buffer, 0, 128);
        memcpy(local_buffer, str, len);
        local_buffer[len + 1] = 0;
        current = local_buffer;
    }

    if (current == NULL || *current == '\0')
    {
        return NULL;
    }

    char *token = current;

    while (*current != '\0' && strchr(delim, *current) == NULL)
    {
        ++current;
    }

    if (*current != '\0')
    {
        *current = '\0';
        ++current;
    }

    return token;
}