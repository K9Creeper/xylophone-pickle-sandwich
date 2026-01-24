/// -----------------------
/// @file storage-manager.c

#include <kernel/storage/storage-manager.h>

#include <string.h>
#include <memory.h>

static uint32_t next_controller_uid = 1;

storage_manager_result_t storage_manager_setup(storage_manager_t *storage_manager)
{
    if (!storage_manager || storage_manager->is_initialized)
        return STORAGE_MANAGER_INVALID;

    memset((uint8_t *)storage_manager->controllers, 0, sizeof(storage_manager->controllers));
    storage_manager->controller_count = 0;

    storage_manager->is_initialized = 1;

    return STORAGE_MANAGER_OK;
}

storage_manager_result_t storage_manager_add_controller(storage_manager_t* sm, storage_controller_t controller) {
    if (!sm || !sm->is_initialized) return STORAGE_MANAGER_INVALID;

    if (sm->controller_count >= STORAGE_MANAGER_MAX_CONTROLLERS) return STORAGE_MANAGER_FULL;

    for (uint8_t i = 0; i < sm->controller_count; ++i) {
        if (sm->controllers[i].uid == controller.uid) {
            return STORAGE_MANAGER_EXISTS;
        }
    }

    controller.uid = next_controller_uid++;
    
    sm->controllers[sm->controller_count++] = controller;

    return STORAGE_MANAGER_OK;
}

storage_manager_result_t storage_manager_remove_controller(storage_manager_t* sm, uint8_t controller_uid) {
    if (!sm || !sm->is_initialized) return STORAGE_MANAGER_INVALID;

    int index = -1;
    for (uint8_t i = 0; i < sm->controller_count; ++i) {
        if (sm->controllers[i].uid == controller_uid) {
            index = i;
            break;
        }
    }

    if (index == -1) return STORAGE_MANAGER_INVALID;

    for (uint8_t i = index; i < sm->controller_count - 1; ++i) {
        sm->controllers[i] = sm->controllers[i + 1];
    }

    sm->controller_count--;
    return STORAGE_MANAGER_OK;
}

storage_controller_t* storage_manager_get_controller(storage_manager_t* sm, uint8_t controller_uid) {
    if (!sm || !sm->is_initialized) return NULL;

    for (uint8_t i = 0; i < sm->controller_count; ++i) {
        if (sm->controllers[i].uid == controller_uid) {
            return &sm->controllers[i];
        }
    }

    return NULL;
}

void storage_manager_iterate(storage_manager_t* sm, void (*callback)(storage_controller_t*, void*), void* arg) {
    if (!sm || !sm->is_initialized || !callback) return;

    for (uint8_t i = 0; i < sm->controller_count; ++i) {
        callback(&sm->controllers[i], arg);
    }
}