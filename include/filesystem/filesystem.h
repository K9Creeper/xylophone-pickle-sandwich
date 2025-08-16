/// ------------
/// filesystem.h
/// @brief This file 

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include <data-structures/filesystem/filesystem.h>

extern void filesystem_init(filesystem_t* filesystem, disk_manager_t* disk_manager);
extern filesystem_mount_t* filesystem_mount(filesystem_t* filesystem, int disk_num);

#endif