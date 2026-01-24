/// --------------------------
/// @file storage-controller.h

#ifndef STORAGE_STORAGE_CONTROLLER_H
#define STORAGE_STORAGE_CONTROLLER_H

#include <stdint.h>

#define STORAGE_CONTROLLER_FLAG_BUSY      (1 << 0) 
#define STORAGE_CONTROLLER_FLAG_READING   (1 << 1)  
#define STORAGE_CONTROLLER_FLAG_WRITING   (1 << 2) 
#define STORAGE_CONTROLLER_FLAG_ERROR     (1 << 3)

#define STORAGE_CONTROLLER_SET_FLAG(status, flag)   (status |= (flag))
#define STORAGE_CONTROLLER_CLEAR_FLAG(status, flag) (status &= ~(flag))
#define STORAGE_CONTROLLER_HAS_FLAG(status, flag)   ((status & (flag)) != 0)

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

    uint32_t (*status)(void *impl);
} storage_controller_ops_t;

typedef struct storage_controller_s {
    const char *name;
    void *impl;
    const storage_controller_ops_t *ops;
    uint32_t flags;
    
    uint32_t uid; 
} storage_controller_t;

#endif