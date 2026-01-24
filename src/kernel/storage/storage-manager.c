/// -----------------------
/// @file storage-manager.c

#include <kernel/storage/storage-manager.h>

void storage_manager_setup(storage_manager_t *storage_manager)
{
    if (!storage_manager || storage_manager->is_initialized)
        return;

    storage_manager->is_initialized = 1;
}

static inline uint8_t storage_manager_add_controller(
    storage_manager_t* storage_manager, 
    storage_controller_t* controller
) {
    for (uint8_t i = 0; i < storage_manager->controller_count; i++) {
        if (storage_manager->controllers[i] == controller) {
            return 2;
        }
    }

    if (storage_manager->controller_count >= STORAGE_MANAGER_MAX_CONTROLLERS) {
        return 1;
    }

    storage_manager->controllers[storage_manager->controller_count] = controller;
    storage_manager->controller_count++;
    return 0;
}


static inline uint8_t storage_manager_remove_controller(
    storage_manager_t* storage_manager, 
    storage_controller_t* controller
) {
    for (uint8_t i = 0; i < storage_manager->controller_count; i++) {
        if (storage_manager->controllers[i] == controller) {
            for (uint8_t j = i; j < storage_manager->controller_count - 1; j++) {
                storage_manager->controllers[j] = storage_manager->controllers[j + 1];
            }
            storage_manager->controllers[storage_manager->controller_count - 1] = (storage_controller_t*)0;
            storage_manager->controller_count--;
            return 0;
        }
    }
    return 1;
}

uint8_t storage_manager_mount(storage_manager_t *storage_manager, storage_controller_t *controller)
{
    if (!storage_manager) return 3;

    return storage_manager_add_controller(storage_manager, controller);
}