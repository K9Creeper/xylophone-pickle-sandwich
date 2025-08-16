/// --------------
/// disk-manager.c
/// @brief This file

#include "disk-manager.h"

#include <disk-manager/disk-manager.h>
#include <data-structures/kernel/kernel-context.h>

// kernel-main.c
extern kernel_context_t* kernel_context;

void kdisk_manager_init(void){
    disk_manager_init(&kernel_context->disk_manager);
}