#include "terminal.h"

#include <stddef.h>
#include <memory.h>

static char input_buffer[TERMINAL_INPUT_BUFFER_SIZE];
static int input_pos = 0;
static bool input_done = false;

bool terminal_add_key(char c)
{
    if (input_done)
        return false;

    switch (c)
    {
        case '\n':
        case '\r':
            input_buffer[input_pos] = '\0';
            input_done = true;
            return true;

        case '\b':
        case 0x7F: // Backspace / Delete
            if (input_pos > 0)
            {
                input_pos--;
                input_buffer[input_pos] = '\0';
            }
            return true;

        default:
            if (input_pos < TERMINAL_INPUT_BUFFER_SIZE - 1)
            {
                input_buffer[input_pos++] = c;
                input_buffer[input_pos] = '\0';
            }
            return true;
    }
}

bool terminal_get_input(char *dest, size_t dest_size)
{
    if (!input_done || dest == NULL || dest_size == 0)
        return false;

    size_t copy_len = (input_pos < dest_size - 1) ? input_pos : dest_size - 1;

    memcpy(dest, input_buffer, copy_len);
    dest[copy_len] = '\0';

    input_pos = 0;
    input_done = false;
    input_buffer[0] = '\0';

    return true;
}