/// -----------------------
/// @file storage-manager.h

#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <stdint.h>

#include <kernel/data-structures/storage/storage-controller.h>

#define STORAGE_MANAGER_MAX_CONTROLLERS 8

typedef struct storage_manager_s {
    storage_controller_t* controllers[STORAGE_MANAGER_MAX_CONTROLLERS];
    uint8_t controller_count;

    uint8_t is_initialized;
} storage_manager_t;

#endif