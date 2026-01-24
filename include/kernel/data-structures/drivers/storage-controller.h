/// ------------------------
/// @file drive-controller.h

#ifndef DRIVER_STORAGE_CONTROLLER_H
#define DRIVER_STORAGE_CONTROLLER_H

#include <stdint.h>

typedef int32_t (*storage_read_t)(
    void *impl,
    uint64_t lba,
    uint32_t sector_count,
    void *buffer
);

typedef int32_t (*storage_write_t)(
    void *impl,
    uint64_t lba,
    uint32_t sector_count,
    const void *buffer
);

typedef struct storage_controller_ops_s {
    storage_read_t  read;
    storage_write_t write;
    uint32_t (*sector_size)(void *impl);
    uint64_t (*sector_count)(void *impl);
} storage_controller_ops_t;

typedef struct storage_controller_s {
    const char *name;
    void *impl;
    const storage_controller_ops_t *ops;
    uint32_t flags;
} storage_controller_t;

#endif