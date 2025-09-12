/// ---------------------
/// @file window-server.c

#include "window-server.h"

#include <memory.h>
#include <string.h>

#include <kernel/util.h>

#include <kernel/data-structures/window-server/window-server.h>
#include <kernel/data-structures/window-server/window.h>
#include <kernel/window-server/window-list.h>

static window_list_t window_list;

uint8_t window_server_init(void)
{
    window_t **data = (window_t **)kernel_malloc(MAX_CONCURRENT_WINDOWS * sizeof(window_t *));
    if (data == NULL)
        return 1;
    int *free_ids = (int *)kernel_malloc(MAX_CONCURRENT_WINDOWS * sizeof(int));
    if (free_ids == NULL)
        return 1;

    window_list_init(&window_list, data, free_ids, MAX_CONCURRENT_WINDOWS);

    return 0;
}

static void create_window(){

}