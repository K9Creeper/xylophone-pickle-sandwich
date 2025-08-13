/// -------
/// mouse.h
/// @brief This file declares and defines important strucuters relating to the mouse.

#ifndef DRIVERS_MOUSE_H
#define DRIVERS_MOUSE_H

#include <data-structures/drivers/mouse.h>

extern void mouse_init(void);

extern mouse_info_t mouse_get_info(void);

#endif