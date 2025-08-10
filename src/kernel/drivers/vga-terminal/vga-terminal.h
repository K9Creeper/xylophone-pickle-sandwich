/// --------------
/// vga-terminal.h
/// @brief This file declares the functions of the VGA Terminal Display.

#ifndef VGA_TERMINAL_H
#define VGA_TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

#define DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS (0xB8000 + 0xC0000000)

#define VGA_TERMINAL_WIDTH 80
#define VGA_TERMINAL_HEIGHT 25

typedef enum {
    VGA_TERMINAL_COLOR_BLACK = 0,
    VGA_TERMINAL_COLOR_BLUE = 1,
    VGA_TERMINAL_COLOR_GREEN = 2,
    VGA_TERMINAL_COLOR_CYAN = 3,
    VGA_TERMINAL_COLOR_RED = 4,
    VGA_TERMINAL_COLOR_MAGENTA = 5,
    VGA_TERMINAL_COLOR_BROWN = 6,
    VGA_TERMINAL_COLOR_LIGHT_GREY = 7,
    VGA_TERMINAL_COLOR_DARK_GREY = 8,
    VGA_TERMINAL_COLOR_LIGHT_BLUE = 9,
    VGA_TERMINAL_COLOR_LIGHT_GREEN = 10,
    VGA_TERMINAL_COLOR_LIGHT_CYAN = 11,
    VGA_TERMINAL_COLOR_LIGHT_RED = 12,
    VGA_TERMINAL_COLOR_LIGHT_MAGENTA = 13,
    VGA_TERMINAL_COLOR_LIGHT_BROWN = 14,
    VGA_TERMINAL_COLOR_WHITE = 15
} vga_terminal_color_enum_t;

extern bool vga_terminal_is_using;

extern void vga_terminal_init(uint32_t buffer_address, vga_terminal_color_enum_t foreground_color, vga_terminal_color_enum_t background_color);
extern void vga_terminal_show_cursor(bool show);
extern void vga_terminal_destroy(void);
extern void vga_terminal_clear(void);
extern void vga_terminal_write(const char* data, uint32_t size);
extern void vga_terminal_write_string(const char *format, ...);

#endif