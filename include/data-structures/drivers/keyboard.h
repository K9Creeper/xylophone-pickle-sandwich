/// ----------
/// keyboard.h
/// @brief This file declares and defines important strucuters relating to the keyboard.
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

typedef struct keyboard_key_s{
    uint16_t index;
    char* keyname;
    char value;

    bool is_pressed;
    bool previously_pressed;
}keyboard_key_t;

typedef keyboard_key_t* keyboard_map_t;

typedef void (*keyboard_input_handle_t)(keyboard_key_t keyboard_key, const keyboard_map_t keyboard_map);

#endif