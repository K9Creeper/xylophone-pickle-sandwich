/// ------------
/// filesystem.h
/// @brief 

#ifndef KERNEL_FILESYSTEM_H
#define KERNEL_FILESYSTEM_H
#include <filesystem/filesystem.h>

extern void kernel_filesystem_init(void);
extern file_info_t* kernel_filesystem_get_file(const char* path);
extern file_info_t* kernel_filesystem_list_dir(const char* path, uint32_t *max_entries);

#endif