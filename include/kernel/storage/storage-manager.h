/// -----------------------
/// @file storage-manager.h

#ifndef _STORAGE_MANAGER_H
#define _STORAGE_MANAGER_H

#include <kernel/data-structures/storage/storage-manager.h>

extern void storage_manager_setup(storage_manager_t* storage_manager);

extern uint8_t storage_manager_mount(storage_manager_t* storage_manager, storage_controller_t* controller);

#endif