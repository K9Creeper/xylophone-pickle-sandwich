#include "terminal.h"

#include <stddef.h>
#include <memory.h>

static char input_buffer[TERMINAL_INPUT_BUFFER_SIZE];
static int input_pos = 0;
static bool input_done = false;

void terminal_add_key(char c)
{
    if (input_done)
        return;

    if (c == '\n' || c == '\r')
    {
        input_buffer[input_pos] = '\0';
        input_done = true;
        return;
    }

    if (c == '\b' || c == 0x7F)
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

bool terminal_get_input(char *dest, uint32_t dest_size)
{
    if (!input_done)
        return false;

    if (dest_size == 0)
        return false;

    if (dest_size <= (uint32_t)input_pos)
        return false;

    memcpy(dest, input_buffer, input_pos + 1);

    input_pos = 0;
    input_done = false;

    return true;
}
