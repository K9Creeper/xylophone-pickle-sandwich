#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

#define TERMINAL_INPUT_BUFFER_SIZE 128

extern void terminal_add_key(char c);
extern bool terminal_get_input(char *dest, uint32_t dest_size);

#endif