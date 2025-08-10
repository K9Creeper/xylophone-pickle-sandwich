/// ----------
/// keyboard.c
/// @brief This file implements the keyboard driver.

#include "keyboard.h"

#include <stddef.h>
#include <registers.h>
#include <memory.h>

#include <data-structures/kernel/kernel-interrupts.h>
#include "../../interrupts/interrupt-request.h"

#define NUM_OF_KEYBOARD_HANDLES 16

// US QWERTY keymap
static keyboard_key_t keymap[128] = {
    {0, (char *)"None", '\0',false, false},
    {1, (char *)"Escape", 27, false, false},
    {2, (char *)"1", '1', false, false},
    {3, (char *)"2", '2', false, false},
    {4, (char *)"3", '3', false, false},
    {5, (char *)"4", '4', false, false},
    {6, (char *)"5", '5', false, false},
    {7, (char *)"6", '6', false, false},
    {8, (char *)"7", '7', false, false},
    {9, (char *)"8", '8', false, false},
    {10, (char *)"9", '9', false, false},
    {11, (char *)"0", '0', false, false},
    {12, (char *)"Minus", '-', false, false},
    {13, (char *)"Equal", '=', false, false},
    {14, (char *)"Backspace", '\b', false, false},
    {15, (char *)"Tab", '\t', false, false},
    {16, (char *)"Q", 'q', false, false},
    {17, (char *)"W", 'w', false, false},
    {18, (char *)"E", 'e', false, false},
    {19, (char *)"R", 'r', false, false},
    {20, (char *)"T", 't', false, false},
    {21, (char *)"Y", 'y', false, false},
    {22, (char *)"U", 'u', false, false},
    {23, (char *)"I", 'i', false, false},
    {24, (char *)"O", 'o', false, false},
    {25, (char *)"P", 'p', false, false},
    {26, (char *)"Left Bracket", '[', false, false},
    {27, (char *)"Right Bracket", ']', false, false},
    {28, (char *)"Enter", '\n', false, false},
    {29, (char *)"Left Control", '\0',false, false},
    {30, (char *)"A", 'a', false, false},
    {31, (char *)"S", 's', false, false},
    {32, (char *)"D", 'd', false, false},
    {33, (char *)"F", 'f', false, false},
    {34, (char *)"G", 'g', false, false},
    {35, (char *)"H", 'h', false, false},
    {36, (char *)"J", 'j', false, false},
    {37, (char *)"K", 'k', false, false},
    {38, (char *)"L", 'l', false, false},
    {39, (char *)"Semicolon", ';', false, false},
    {40, (char *)"Apostrophe", '\'', false, false},
    {41, (char *)"Backtick", '`', false, false},
    {42, (char *)"Left Shift", '\0',false, false},
    {43, (char *)"Backslash", '\\', false, false},
    {44, (char *)"Z", 'z', false, false},
    {45, (char *)"X", 'x', false, false},
    {46, (char *)"C", 'c', false, false},
    {47, (char *)"V", 'v', false, false},
    {48, (char *)"B", 'b', false, false},
    {49, (char *)"N", 'n', false, false},
    {50, (char *)"M", 'm', false, false},
    {51, (char *)"Comma", ',', false, false},
    {52, (char *)"Period", '.', false, false},
    {53, (char *)"Slash", '/', false, false},
    {54, (char *)"Right Shift", '\0',false, false},
    {55, (char *)"Multiply", '*', false, false},
    {56, (char *)"Left Alt", '\0',false, false},
    {57, (char *)"Spacebar", ' ', false, false},
    {58, (char *)"Caps Lock", '\0',false, false},
    {59, (char *)"F1", '\0',false, false},
    {60, (char *)"F2", '\0',false, false},
    {61, (char *)"F3", '\0',false, false},
    {62, (char *)"F4", '\0',false, false},
    {63, (char *)"F5", '\0',false, false},
    {64, (char *)"F6", '\0',false, false},
    {65, (char *)"F7", '\0',false, false},
    {66, (char *)"F8", '\0',false, false},
    {67, (char *)"F9", '\0',false, false},
    {68, (char *)"F10", '\0',false, false},
    {69, (char *)"Num Lock", '\0',false, false},
    {70, (char *)"Scroll Lock", '\0',false, false},
    {71, (char *)"Home", '\0',false, false},
    {72, (char *)"Up Arrow", '\0',false, false},
    {73, (char *)"Page Up", '\0',false, false},
    {74, (char *)"Num Minus", '-', false, false},
    {75, (char *)"Left Arrow", '\0',false, false},
    {76, (char *)"Center Arrow", '\0',false, false},
    {77, (char *)"Right Arrow", '\0',false, false},
    {78, (char *)"Num Plus", '+', false, false},
    {79, (char *)"End", '\0',false, false},
    {80, (char *)"Down Arrow", '\0',false, false},
    {81, (char *)"Page Down", '\0',false, false},
    {82, (char *)"Insert", '\0',false, false},
    {83, (char *)"Delete", '\0',false, false},
    {84, (char *)"Num 0", '0', false, false},
    {85, (char *)"Num 1", '1', false, false},
    {86, (char *)"Num 2", '2', false, false},
    {87, (char *)"Num 3", '3', false, false},
    {88, (char *)"Num 4", '4', false, false},
    {89, (char *)"Num 5", '5', false, false},
    {90, (char *)"Num 6", '6', false, false},
    {91, (char *)"Num 7", '7', false, false},
    {92, (char *)"Num 8", '8', false, false},
    {93, (char *)"Num 9", '9', false, false},
    {94, (char *)"Num Divide", '/', false, false},
    {95, (char *)"Num Multiply", '*', false, false},
    {96, (char *)"Num Subtract", '-', false, false},
    {97, (char *)"Num Add", '+', false, false},
    {98, (char *)"Num Enter", '\n', false, false},
    {99, (char *)"None", '\0',false, false},
    {100, (char *)"None", '\0',false, false},
    {101, (char *)"None", '\0',false, false},
    {102, (char *)"None", '\0',false, false},
    {103, (char *)"None", '\0',false, false},
    {104, (char *)"None", '\0',false, false},
    {105, (char *)"None", '\0',false, false},
    {106, (char *)"None", '\0',false, false},
    {107, (char *)"None", '\0',false, false},
    {108, (char *)"None", '\0',false, false},
    {109, (char *)"None", '\0',false, false},
    {110, (char *)"None", '\0',false, false},
    {111, (char *)"None", '\0',false, false},
    {112, (char *)"None", '\0',false, false},
    {113, (char *)"None", '\0',false, false},
    {114, (char *)"None", '\0',false, false},
    {115, (char *)"None", '\0',false, false},
    {116, (char *)"None", '\0',false, false},
    {117, (char *)"None", '\0',false, false},
    {118, (char *)"None", '\0',false, false},
    {119, (char *)"None", '\0',false, false},
    {120, (char *)"None", '\0',false, false},
    {121, (char *)"None", '\0',false, false},
    {122, (char *)"None", '\0',false, false},
    {123, (char *)"None", '\0',false, false},
    {124, (char *)"None", '\0',false, false},
    {125, (char *)"None", '\0',false, false},
    {126, (char *)"None", '\0',false, false},
    {127, (char *)"None", '\0',false, false},
};
static keyboard_input_handle_t input_handles[NUM_OF_KEYBOARD_HANDLES];

static void keyboard_handler(registers_t *regs);

void keyboard_init(void)
{
    for (int i = 0; i < 128; i++) { keymap[i].is_pressed = false; keymap[i].previously_pressed = false; }
    for (int i = 0; i < NUM_OF_KEYBOARD_HANDLES; i++) { input_handles[i] = NULL; }

    kernel_interrupt_request_set_handle(1, keyboard_handler);
}

int keyboard_add_input_handle(keyboard_input_handle_t handle){
    for (int i = 0; i < NUM_OF_KEYBOARD_HANDLES; i++)
    {
        if (input_handles[i] != NULL)
            continue;

        input_handles[i] = handle;
        return i;
    }

    return -1;
}

void keyboard_remove_input_handle(int index){
    if (index < 0 || index >= NUM_OF_KEYBOARD_HANDLES)
        return;

    input_handles[index] = NULL;
}

keyboard_key_t keyboard_get_key(keyboard_keycode_t keycode){
    return keymap[keycode];
}

static void keyboard_handler(registers_t *regs)
{
    const uint8_t scancode = inportb(0x60);

    if ((scancode & 128) == 128)
    {
        keymap[scancode - 128].previously_pressed = keymap[scancode - 128].is_pressed;
        keymap[scancode - 128].is_pressed = false;
    }
    else
    {
        keymap[scancode].previously_pressed = keymap[scancode].is_pressed;
        keymap[scancode].is_pressed = true;
    }

    for (int i = 0; i < NUM_OF_KEYBOARD_HANDLES; i++)
    {
        if (!input_handles[i])
            continue;

        if ((scancode & 128) == 128)
        {
            ((keyboard_input_handle_t)(input_handles[i]))(keymap[scancode - 128], (const keyboard_map_t)keymap);
        }
        else
        {
            ((keyboard_input_handle_t)(input_handles[i]))(keymap[scancode], (const keyboard_map_t)keymap);
        }
    }
}