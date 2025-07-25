/// --------------
/// vga-terminal.c
/// @brief This file defines the functions of the VGA Terminal Display.

#include "vga-terminal.h"

#include <stddef.h>
#include <stdarg.h>
#include <string.h>

bool vga_terminal_is_using = false;

static uint16_t* vga_terminal_buffer = NULL;
static uint16_t vga_terminal_row;
static uint16_t vga_terminal_column;
static uint8_t vga_terminal_color;

static uint8_t entry_color(vga_terminal_color_enum_t fg, vga_terminal_color_enum_t bg) {
    return (fg & 0x0F) | ((bg & 0x07) << 4);  // Mask to avoid blinking
}

static uint16_t entry(unsigned char character, uint8_t color){
    return (uint16_t)character | (uint16_t)color << 8;
}

static void put_entry_at(char c, uint8_t color, uint16_t x, uint16_t y)
{
    const uint16_t index = y * VGA_TERMINAL_WIDTH + x;
	vga_terminal_buffer[index] = entry(c, color);
}

static void put_char(char c){
    put_entry_at(c, vga_terminal_color, vga_terminal_column, vga_terminal_row);
    if(++vga_terminal_column == VGA_TERMINAL_WIDTH){
        vga_terminal_column = 0;
        if (++vga_terminal_row == VGA_TERMINAL_HEIGHT)
			vga_terminal_row = 0;
    }
}

static void write(const char* data, size_t size){
    for (size_t i = 0; i < size; i++)
	{
		switch (data[i])
		{
		case '\n':
			vga_terminal_row++;
			vga_terminal_column = 0;
			break;

		case '\t':
			vga_terminal_column+=8;
			
			break;

		case '\b':
			vga_terminal_column--;

			break;
		default:
			put_char(data[i]);
			break;
		}
	}
}

static void clear(void){
    for (uint16_t y = 0; y < VGA_TERMINAL_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < VGA_TERMINAL_WIDTH; x++)
		{
			const uint16_t index = y * VGA_TERMINAL_WIDTH + x;
			vga_terminal_buffer[index] = entry(' ', vga_terminal_color);
		}
	}
}

void vga_terminal_init(uint32_t buffer_address, vga_terminal_color_enum_t foreground_color, vga_terminal_color_enum_t background_color){
    vga_terminal_buffer = (uint16_t*)buffer_address;

    vga_terminal_row = 0;
    vga_terminal_column = 0;
    vga_terminal_color = entry_color(foreground_color, background_color);
    vga_terminal_is_using = true;

    clear();
}

void vga_terminal_destroy(void){    
    vga_terminal_row = 0;
    vga_terminal_column = 0;

    clear();
    
    vga_terminal_is_using = false;
}

void vga_terminal_write_string(const char *format, ...){
    if(!vga_terminal_buffer || !vga_terminal_is_using)
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
			write(buffer, strlen(buffer));
			buffer_index = 0;
		}
	}

	buffer[buffer_index] = '\0';
	write(buffer, strlen(buffer));

	va_end(args);
}