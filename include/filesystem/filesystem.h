/// ------------
/// filesystem.h
/// @brief This file 

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <data-structures/filesystem/filesystem.h>

extern void filesystem_init(filesystem_t* filesystem, disk_manager_t* disk_manager);
extern filesystem_mount_t* filesystem_mount(filesystem_t* filesystem, int disk_num);
extern bool filesystem_get_entry(filesystem_t* filesystem, const char* path, fat_file_entry_t * out);

// @param max_entries: like this will return how much there actually is if the number passed first is higher than the output
extern bool filesystem_get_entries(filesystem_t* filesystem, const char* path, fat_file_entry_t *out_entries, uint32_t* max_entries);

#endif