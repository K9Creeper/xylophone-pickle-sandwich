/// ------------------------
/// @file drive-controller.h

#ifndef DRIVER_DRIVE_CONTROLLER_H
#define DRIVER_DRIVE_CONTROLLER_H

#include <stdint.h>

typedef int32_t (*drive_read_t)(
    void *impl,
    uint64_t lba,
    uint32_t sector_count,
    void *buffer
);

typedef int32_t (*drive_write_t)(
    void *impl,
    uint64_t lba,
    uint32_t sector_count,
    const void *buffer
);

typedef struct drive_controller_ops_s {
    drive_read_t  read;
    drive_write_t write;
    uint32_t (*sector_size)(void *impl);
    uint64_t (*sector_count)(void *impl);
} drive_controller_ops_t;

typedef struct drive_controller_s {
    const char *name;
    void *impl;
    const drive_controller_ops_t *ops;
    uint32_t flags;
} drive_controller_t;

#endif