/// -------
/// mouse.h
/// @brief This file declares and defines important strucuters relating to the mouse.

#ifndef DRIVERS_MOUSE_H
#define DRIVERS_MOUSE_H

#include <data-structures/drivers/mouse.h>

extern void mouse_init(void);

extern int mouse_add_input_handle(mouse_input_handle_t handle);
extern void mouse_remove_input_handle(int index);

extern mouse_info_t mouse_get_info(void);

#endif