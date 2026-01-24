/// -----------------------
/// @file storage-manager.h

#ifndef _STORAGE_MANAGER_H
#define _STORAGE_MANAGER_H

#include <kernel/data-structures/storage/storage-manager.h>

extern storage_manager_result_t storage_manager_setup(storage_manager_t* storage_manager);

extern storage_controller_t* storage_manager_get_controller(storage_manager_t* sm, uint8_t controller_uid);
extern void storage_manager_iterate(storage_manager_t* sm, void (*callback)(storage_controller_t*, void*), void* arg);

extern storage_manager_result_t storage_manager_add_controller(storage_manager_t* sm, storage_controller_t controller);
extern storage_manager_result_t storage_manager_remove_controller(storage_manager_t* sm, uint8_t controller_uid);

#endif