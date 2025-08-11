/// --------
/// mouse.h
/// @brief This file declares and defines important strucuters relating to the mouse.

#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

#define MOUSE_LEFT_BUTTON(flag) (flag & 0x1)
#define MOUSE_RIGHT_BUTTON(flag) (flag & 0x2)
#define MOUSE_MIDDLE_BUTTON(flag) (flag & 0x4)

typedef struct mouse_info_s{
    int x;
    int y;

    int delta_x;
    int delta_y;

    uint8_t prev_state[3];
    uint8_t curr_state[3];
} mouse_info_t;

typedef void (*mouse_input_handle_t)(const mouse_info_t* info);

#endif