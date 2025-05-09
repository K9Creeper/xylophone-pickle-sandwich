/// ---------------------------
/// vga-terminal.hpp
/// @brief This file defines the functions of the VGA Terminal Display.

#include "vga-terminal.hpp"

#include <stdarg.h>

#include "../../../helpers/string.h"

#define VGA_WIDTH   80
#define VGA_HEIGHT  25

static bool bInitialized = false;

static uint16_t VGA_Terminal_row;
static uint16_t VGA_Terminal_column;
static uint8_t VGA_Terminal_color;
static uint16_t *VGA_Terminal_buffer = nullptr;

static uint8_t VGA_Terminal_entry_color(VGA_Color fg, VGA_Color bg)
{
	return fg | bg << 4;
}

static uint16_t VGA_Terminal_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

static void VGA_Terminal_putentryat(char c, uint8_t color, uint16_t x, uint16_t y)
{
	const uint16_t index = y * VGA_WIDTH + x;
	VGA_Terminal_buffer[index] = VGA_Terminal_entry(c, color);
}

static void VGA_Terminal_putchar(char c)
{
	VGA_Terminal_putentryat(c, VGA_Terminal_color, VGA_Terminal_column, VGA_Terminal_row);
	if (++VGA_Terminal_column == VGA_WIDTH)
	{
		VGA_Terminal_column = 0;
		if (++VGA_Terminal_row == VGA_HEIGHT)
			VGA_Terminal_row = 0;
	}
}

static void VGA_Terminal_write(const char *data, uint16_t size)
{
	for (uint16_t i = 0; i < size; i++)
	{
		switch (data[i])
		{
		case '\n':
			VGA_Terminal_row++;
			VGA_Terminal_column = 0;
			break;

		case '\t':
			VGA_Terminal_column+=8;
			
			break;

		case '\b':
			VGA_Terminal_column--;

			break;
		default:
			VGA_Terminal_putchar(data[i]);
			break;
		}
	}
}

void Kernel::Misc::VGA_Terminal_writestring(const char *format, ...)
{
	if (!bInitialized)
		return;
	char buffer[256];
	int buffer_index = 0;

	va_list args;
	va_start(args, format);

	for (const char *ptr = format; *ptr != '\0'; ++ptr)
	{
		if (*ptr == '%')
		{
			++ptr;
			switch (*ptr)
			{
			case 'd':
			{
				int num = va_arg(args, int);
				char num_str[32];
				itoa(num, num_str, 10);
				for (char *c = num_str; *c != '\0'; ++c)
				{
					buffer[buffer_index++] = *c;
				}
				break;
			}
			case 'D':
			{
				uint32_t num = va_arg(args, uint32_t);
				char num_str[32];
				uitoa(num, num_str, 10);
				for (char *c = num_str; *c != '\0'; ++c)
				{
					buffer[buffer_index++] = *c;
				}
				break;
			}
			case 'x':
			{
				int num = va_arg(args, int);
				char num_str[32];
				itoa(num, num_str, 16);
				for (char *c = num_str; *c != '\0'; ++c)
				{
					buffer[buffer_index++] = *c;
				}
				break;
			}
			case 'X':
			{
				uint32_t num = va_arg(args, uint32_t);
				char num_str[32];
				uitoa(num, num_str, 16);
				for (char *c = num_str; *c != '\0'; ++c)
				{
					buffer[buffer_index++] = *c;
				}
				break;
			}
			case 's':
			{
				const char *str = va_arg(args, const char *);
				for (const char *c = str; *c != '\0'; ++c)
				{
					buffer[buffer_index++] = *c;
				}
				break;
			}
			case '%':
				buffer[buffer_index++] = '%';
				break;
			default:
				buffer[buffer_index++] = '%';
				buffer[buffer_index++] = *ptr;
				break;
			}
		}
		else
		{
			buffer[buffer_index++] = *ptr;
		}

		if (buffer_index >= sizeof(buffer) - 1)
		{
			buffer[buffer_index] = '\0';
			VGA_Terminal_write(buffer, strlen(buffer));
			buffer_index = 0;
		}
	}

	buffer[buffer_index] = '\0';
	VGA_Terminal_write(buffer, strlen(buffer));

	va_end(args);
}

bool Kernel::Misc::VGA_isUsing(){
	return bInitialized;
}

void Kernel::Misc::VGA_Terminal_Init(uint32_t bufferAddress)
{
	VGA_Terminal_buffer = reinterpret_cast<uint16_t *>(bufferAddress);

	VGA_Terminal_row = 0;
	VGA_Terminal_column = 0;
	VGA_Terminal_color = VGA_Terminal_entry_color(VGA_Color_Light_Grey, VGA_Color_Black);

	for (uint16_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < VGA_WIDTH; x++)
		{
			const uint16_t index = y * VGA_WIDTH + x;
			VGA_Terminal_buffer[index] = VGA_Terminal_entry(' ', VGA_Terminal_color);
		}
	}

	bInitialized = true;
}

void Kernel::Misc::VGA_Terminal_Destroy(){
	VGA_Terminal_row = 0;
	VGA_Terminal_column = 0;
	VGA_Terminal_color = VGA_Terminal_entry_color(VGA_Color_Light_Grey, VGA_Color_Black);

	for (uint16_t y = 0; y < VGA_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < VGA_WIDTH; x++)
		{
			const uint16_t index = y * VGA_WIDTH + x;
			VGA_Terminal_buffer[index] = VGA_Terminal_entry(' ', VGA_Terminal_color);
		}
	}

	bInitialized = false;
}