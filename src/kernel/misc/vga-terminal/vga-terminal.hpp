/// ---------------------------
/// vga-terminal.hpp
/// @brief This file defines the structures to the VGA Terminal Display.

#pragma once

#include <stdint.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  (0xB8000 + 0xC0000000)

enum VGA_Color : uint16_t {
	VGA_Color_Black = 0,
	VGA_Color_Blue = 1,
	VGA_Color_Green = 2,
	VGA_Color_Cyan = 3,
	VGA_Color_Red = 4,
	VGA_Color_Magenta = 5,
	VGA_Color_Brown = 6,
	VGA_Color_Light_Grey = 7,
	VGA_Color_Dark_Grey = 8,
	VGA_Color_Light_Blue = 9,
	VGA_Color_Light_Green = 10,
	VGA_Color_Light_Cyan = 11,
	VGA_Color_Light_Red = 12,
	VGA_Color_Light_Magenta = 13,
	VGA_Color_Light_Brown = 14,
	VGA_Color_White = 15,
};

namespace Kernel{
    namespace Misc{
		extern void VGA_Terminal_Init();
		extern void VGA_Terminal_writestring(const char *format, ...);
		extern void VGA_Terminal_setcolor(uint8_t color);
	}
}