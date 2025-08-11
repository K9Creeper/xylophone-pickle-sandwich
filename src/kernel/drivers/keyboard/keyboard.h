/// ----------
/// keyboard.h
/// @brief This file declares and defines important strucuters relating to the keyboard.

#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include <data-structures/drivers/keyboard.h>

extern void keyboard_init(void);

extern void keyboard_add_input_handle(keyboard_input_handle_t handle);
extern void keyboard_remove_input_handle(void);

extern keyboard_key_t keyboard_get_key(keyboard_keycode_t keycode);

#endif