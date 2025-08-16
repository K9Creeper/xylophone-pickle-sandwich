/// ------------
/// filesystem.c 
/// @brief 

#include "filesystem.h"
#include <filesystem/filesystem.h>

#include <data-structures/kernel/kernel-context.h>
// kernel-main.c
extern kernel_context_t* kernel_context;

void kernel_filesystem_init(void){
    filesystem_init(&kernel_context->filesystem, &kernel_context->disk_manager);
}