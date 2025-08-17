/// ------------
/// filesystem.c
/// @brief

#include "filesystem.h"

#include <memory.h>
#include <string.h>

#include <data-structures/kernel/kernel-context.h>
// kernel-main.c
extern kernel_context_t *kernel_context;

#include <scheduling/task.h>
#include <syscall.h>

#include <stdio.h>

void kernel_filesystem_init(void)
{
    filesystem_init(&kernel_context->filesystem, &kernel_context->disk_manager);
}

file_info_t *kernel_filesystem_get_file(const char *path)
{
    if (path == NULL)
        return NULL;

    const char *dot = strrchr(path, '.');
    if (dot == NULL || *(dot + 1) == '\0')
    {
        return NULL;
    }

    const char *relative_path = path;
    static char full_path[TASK_MAX_FILESYSTEM_PATH_LENGTH];

    if (path[1] == ':' && path[2] == '/' && (strlen(relative_path) <= 3 && relative_path[3] == '\0'))
    {
        relative_path = path + 3;

        if(relative_path[0] == '\0')
        {
            path = relative_path;
            goto empty_path;
        }
    }
    else if(!(path[1] == ':' && path[2] == '/'))
    {
empty_path:
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

    fat_file_entry_t entry = {0};
    if (!filesystem_get_entry(&kernel_context->filesystem, relative_path, &entry))
    {
        return NULL;
    }

    file_info_t *file_info = malloc(sizeof(file_info_t));
    if (file_info == NULL)
        return NULL;

    char temp[12];

    int i = 0;
    for (int j = 0; j < 8 && entry.entry.name[j] != ' '; j++)
    {
        temp[i++] = entry.entry.name[j];
    }

    if (entry.entry.extension[0] != ' ')
    {
        temp[i++] = '.';
        for (int j = 0; j < 3 && entry.entry.extension[j] != ' '; j++)
        {
            temp[i++] = entry.entry.extension[j];
        }
    }
    temp[i] = '\0';

    memcpy(file_info->name, temp, sizeof(file_info->name));
    file_info->name[sizeof(file_info->name) - 1] = '\0';

    memcpy(file_info->path, relative_path, sizeof(file_info->path));
    
    file_info->attributes = entry.entry.attributes;
    file_info->created = 0;
    file_info->modified = 0;
    file_info->accessed = 0;
    file_info->size = entry.entry.file_size;

    return file_info;
}

file_info_t *kernel_filesystem_list_dir(const char *path, uint32_t *max_entries)
{
    if (path == NULL || max_entries == NULL)
        return NULL;

    const char *relative_path = path;
    static char full_path[TASK_MAX_FILESYSTEM_PATH_LENGTH];

    if (path[1] == ':' && path[2] == '/' && (strlen(relative_path) <= 3 && relative_path[3] == '\0'))
    {
        relative_path = path + 3;

        if(relative_path[0] == '\0')
        {
            path = relative_path;
            goto empty_path;
        }
    }
    else if(!(path[1] == ':' && path[2] == '/'))
    {
empty_path:
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

    fat_file_entry_t *entries = malloc(sizeof(fat_file_entry_t) * (*max_entries));

    if (entries == NULL)
        return NULL;

    if (!filesystem_get_entries(&kernel_context->filesystem, relative_path, entries, max_entries))
    {
        free(entries);
        return NULL;
    }

    file_info_t *file_infos = malloc(sizeof(file_info_t) * (*max_entries));
    if (file_infos == NULL)
    {
        free(entries);
        return NULL;
    }
    
    for (int i = 0; i < *max_entries; i++)
    {
        if(!((entries[i].entry.attributes & FAT_ARCHIVE) || (entries[i].entry.attributes & FAT_DIRECTORY))) continue;

        char temp[12];

        int l = 0;
        for (int j = 0; j < 8 && entries[i].entry.name[j] != ' '; j++)
        {
            temp[l++] = entries[i].entry.name[j];
        }

        if (entries[i].entry.extension[0] != ' ')
        {
            temp[l++] = '.';
            for (int j = 0; j < 3 && entries[i].entry.extension[j] != ' '; j++)
            {
                temp[l++] = entries[i].entry.extension[j];
            }
        }
        temp[l] = '\0';

        memcpy(file_infos[i].name, temp, sizeof(file_infos[i].name));
        file_infos[i].name[sizeof(file_infos[i].name) - 1] = '\0';

        file_infos[i].attributes = entries[i].entry.attributes;
        file_infos[i].created = 0;
        file_infos[i].modified = 0;
        file_infos[i].accessed = 0;
        file_infos[i].size = entries[i].entry.file_size;
    }

    return file_infos;
}

uint32_t kernel_filesystem_read_file(file_info_t* file, void* buffer, uint32_t max_buffer){
    if(file == NULL || buffer == NULL || max_buffer == 0) return 0;
    
    return filesystem_read_data(&kernel_context->filesystem, file->path, buffer, max_buffer);
}