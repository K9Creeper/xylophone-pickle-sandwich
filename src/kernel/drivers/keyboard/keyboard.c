/// ----------
/// keyboard.c
/// @brief This file implements the keyboard driver.

#include "keyboard.h"

#include <stddef.h>
#include <registers.h>
#include <memory.h>

#include <data-structures/kernel/kernel-interrupts.h>
#include "../../interrupts/interrupt-request.h"

// US QWERTY keymap
static keyboard_key_t keymap[MAX_KEYBOARD_KEYS] = {
    {0, (char *)"None", '\0', false},
    {1, (char *)"Escape", 27, false},
    {2, (char *)"1", '1', false},
    {3, (char *)"2", '2', false},
    {4, (char *)"3", '3', false},
    {5, (char *)"4", '4', false},
    {6, (char *)"5", '5', false},
    {7, (char *)"6", '6', false},
    {8, (char *)"7", '7', false},
    {9, (char *)"8", '8', false},
    {10, (char *)"9", '9', false},
    {11, (char *)"0", '0', false},
    {12, (char *)"Minus", '-', false},
    {13, (char *)"Equal", '=', false},
    {14, (char *)"Backspace", '\b', false},
    {15, (char *)"Tab", '\t', false},
    {16, (char *)"Q", 'q', false},
    {17, (char *)"W", 'w', false},
    {18, (char *)"E", 'e', false},
    {19, (char *)"R", 'r', false},
    {20, (char *)"T", 't', false},
    {21, (char *)"Y", 'y', false},
    {22, (char *)"U", 'u', false},
    {23, (char *)"I", 'i', false},
    {24, (char *)"O", 'o', false},
    {25, (char *)"P", 'p', false},
    {26, (char *)"Left Bracket", '[', false},
    {27, (char *)"Right Bracket", ']', false},
    {28, (char *)"Enter", '\n', false},
    {29, (char *)"Left Control", '\0', false},
    {30, (char *)"A", 'a', false},
    {31, (char *)"S", 's', false},
    {32, (char *)"D", 'd', false},
    {33, (char *)"F", 'f', false},
    {34, (char *)"G", 'g', false},
    {35, (char *)"H", 'h', false},
    {36, (char *)"J", 'j', false},
    {37, (char *)"K", 'k', false},
    {38, (char *)"L", 'l', false},
    {39, (char *)"Semicolon", ';', false},
    {40, (char *)"Apostrophe", '\'', false},
    {41, (char *)"Backtick", '`', false},
    {42, (char *)"Left Shift", '\0', false},
    {43, (char *)"Backslash", '\\', false},
    {44, (char *)"Z", 'z', false},
    {45, (char *)"X", 'x', false},
    {46, (char *)"C", 'c', false},
    {47, (char *)"V", 'v', false},
    {48, (char *)"B", 'b', false},
    {49, (char *)"N", 'n', false},
    {50, (char *)"M", 'm', false},
    {51, (char *)"Comma", ',', false},
    {52, (char *)"Period", '.', false},
    {53, (char *)"Slash", '/', false},
    {54, (char *)"Right Shift", '\0', false},
    {55, (char *)"Multiply", '*', false},
    {56, (char *)"Left Alt", '\0', false},
    {57, (char *)"Spacebar", ' ', false},
    {58, (char *)"Caps Lock", '\0', false},
    {59, (char *)"F1", '\0', false},
    {60, (char *)"F2", '\0', false},
    {61, (char *)"F3", '\0', false},
    {62, (char *)"F4", '\0', false},
    {63, (char *)"F5", '\0', false},
    {64, (char *)"F6", '\0', false},
    {65, (char *)"F7", '\0', false},
    {66, (char *)"F8", '\0', false},
    {67, (char *)"F9", '\0', false},
    {68, (char *)"F10", '\0', false},
    {69, (char *)"Num Lock", '\0', false},
    {70, (char *)"Scroll Lock", '\0', false},
    {71, (char *)"Home", '\0', false},
    {72, (char *)"Up Arrow", '\0', false},
    {73, (char *)"Page Up", '\0', false},
    {74, (char *)"Num Minus", '-', false},
    {75, (char *)"Left Arrow", '\0', false},
    {76, (char *)"Center Arrow", '\0', false},
    {77, (char *)"Right Arrow", '\0', false},
    {78, (char *)"Num Plus", '+', false},
    {79, (char *)"End", '\0', false},
    {80, (char *)"Down Arrow", '\0', false},
    {81, (char *)"Page Down", '\0', false},
    {82, (char *)"Insert", '\0', false},
    {83, (char *)"Delete", '\0', false},
    {84, (char *)"Num 0", '0', false},
    {85, (char *)"Num 1", '1', false},
    {86, (char *)"Num 2", '2', false},
    {87, (char *)"Num 3", '3', false},
    {88, (char *)"Num 4", '4', false},
    {89, (char *)"Num 5", '5', false},
    {90, (char *)"Num 6", '6', false},
    {91, (char *)"Num 7", '7', false},
    {92, (char *)"Num 8", '8', false},
    {93, (char *)"Num 9", '9', false},
    {94, (char *)"Num Divide", '/', false},
    {95, (char *)"Num Multiply", '*', false},
    {96, (char *)"Num Subtract", '-', false},
    {97, (char *)"Num Add", '+', false},
    {98, (char *)"Num Enter", '\n', false},
    {99, (char *)"None", '\0', false},
    {100, (char *)"None", '\0', false},
    {101, (char *)"None", '\0', false},
    {102, (char *)"None", '\0', false},
    {103, (char *)"None", '\0', false},
    {104, (char *)"None", '\0', false},
    {105, (char *)"None", '\0', false},
    {106, (char *)"None", '\0', false},
    {107, (char *)"None", '\0', false},
    {108, (char *)"None", '\0', false},
    {109, (char *)"None", '\0', false},
    {110, (char *)"None", '\0', false},
    {111, (char *)"None", '\0', false},
    {112, (char *)"None", '\0', false},
    {113, (char *)"None", '\0', false},
    {114, (char *)"None", '\0', false},
    {115, (char *)"None", '\0', false},
    {116, (char *)"None", '\0', false},
    {117, (char *)"None", '\0', false},
    {118, (char *)"None", '\0', false},
    {119, (char *)"None", '\0', false},
    {120, (char *)"None", '\0', false},
    {121, (char *)"None", '\0', false},
    {122, (char *)"None", '\0', false},
    {123, (char *)"None", '\0', false},
    {124, (char *)"None", '\0', false},
    {125, (char *)"None", '\0', false},
    {126, (char *)"None", '\0', false},
    {127, (char *)"None", '\0', false},
};

static keyboard_input_handle_t input_handle;

static void keyboard_handler(registers_t *regs);

void keyboard_init(void)
{
    for (int i = 0; i < 128; i++)
    {
        keymap[i].is_pressed = false;
    }

    kernel_interrupt_request_set_handle(1, keyboard_handler);
}


void keyboard_add_input_handle(keyboard_input_handle_t handle)
{
    input_handle = handle;
}

void keyboard_remove_input_handle(void)
{
    input_handle = NULL;
}

keyboard_key_t keyboard_get_key(keyboard_keycode_t keycode)
{
    return keymap[keycode];
}

const keyboard_map_t keyboard_get_keyboard(void){
    return (const keyboard_map_t)keymap;
}

static void keyboard_handler(registers_t *regs)
{
    const uint8_t scancode = inportb(0x60);

    bool released = (scancode & 128) != 0;
    uint8_t key_index = released ? (scancode - 128) : scancode;

    keymap[key_index].is_pressed = !released;

    if(input_handle != NULL)
        ((keyboard_input_handle_t)(input_handle))(keymap[key_index], (const keyboard_map_t)keymap);
}