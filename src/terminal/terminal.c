#include "terminal.h"

#include <stddef.h>
#include <memory.h>

static char input_buffer[TERMINAL_INPUT_BUFFER_SIZE];
static int input_pos = 0;
static bool input_done = false;

void terminal_add_key(char c)
{
    if (c == '\n')
    {
        if (input_pos < TERMINAL_INPUT_BUFFER_SIZE)
            input_buffer[input_pos] = '\0';
        else
            input_buffer[TERMINAL_INPUT_BUFFER_SIZE - 1] = '\0';

        input_done = true;
        return;
    }

    if (c == '\b')
    {
        if (input_pos > 0)
        {
            input_pos--;
            input_buffer[input_pos] = '\0';
        }
        return;
    }

    if (input_pos < TERMINAL_INPUT_BUFFER_SIZE - 1)
    {
        input_buffer[input_pos++] = c;
        input_buffer[input_pos] = '\0';
    }
}

bool terminal_get_input(char *dest, uint32_t dest_size) {
    if (!input_done || input_pos <= 0)
        return false;

    // Make sure dest can hold the string including null terminator
    if (dest_size <= input_pos)  // Not enough space
        return false;

    // Copy the input buffer to dest
    memcpy(dest, input_buffer, input_pos);

    // Reset internal state
    input_done = false;
    input_pos = 0;

    return true;
}