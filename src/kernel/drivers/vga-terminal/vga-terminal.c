/// --------------
/// vga-terminal.c
/// @brief This file defines the functions of the VGA Terminal Display.

#include "vga-terminal.h"

#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#include <memory.h>

#include <data-structures/kernel/kernel-context.h>

// kernel-main.c
extern kernel_context_t *kernel_context;

bool vga_terminal_is_using = false;

static uint16_t *vga_terminal_buffer = NULL;
static uint16_t vga_terminal_row;
static uint16_t vga_terminal_column;
static uint8_t vga_terminal_color;

static uint8_t entry_color(vga_terminal_color_enum_t fg, vga_terminal_color_enum_t bg)
{
	return (fg & 0x0F) | ((bg & 0x07) << 4); // Mask to avoid blinking
}

static uint16_t entry(unsigned char character, uint8_t color)
{
	return (uint16_t)character | (uint16_t)color << 8;
}

static void put_entry_at(char c, uint8_t color, uint16_t x, uint16_t y)
{
	const uint16_t index = y * VGA_TERMINAL_WIDTH + x;
	vga_terminal_buffer[index] = entry(c, color);
}

static void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_TERMINAL_WIDTH + x;

	outportb(0x3D4, 0x0F);
	outportb(0x3D5, (uint8_t)(pos & 0xFF));
	outportb(0x3D4, 0x0E);
	outportb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_terminal_putc(char c)
{
	put_entry_at(c, vga_terminal_color, vga_terminal_column, vga_terminal_row);
	if (++vga_terminal_column == VGA_TERMINAL_WIDTH)
	{
		vga_terminal_column = 0;
		if (++vga_terminal_row == VGA_TERMINAL_HEIGHT)
			vga_terminal_row = 0;
	}
}

static void scroll_terminal(void)
{
	for (uint16_t y = 1; y < VGA_TERMINAL_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < VGA_TERMINAL_WIDTH; x++)
		{
			vga_terminal_buffer[(y - 1) * VGA_TERMINAL_WIDTH + x] =
				vga_terminal_buffer[y * VGA_TERMINAL_WIDTH + x];
		}
	}

	// Clear the last line
	for (uint16_t x = 0; x < VGA_TERMINAL_WIDTH; x++)
	{
		put_entry_at(' ', vga_terminal_color, x, VGA_TERMINAL_HEIGHT - 1);
	}
}

void vga_terminal_write(const char *data, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		char c = data[i];

		switch (c)
		{
		case '\n':
			vga_terminal_column = 0;
			vga_terminal_row++;
			break;

		case '\t':
			vga_terminal_column = (vga_terminal_column + 8) & ~(8 - 1);
			if (vga_terminal_column >= VGA_TERMINAL_WIDTH)
			{
				vga_terminal_column = 0;
				vga_terminal_row++;
			}
			break;

		case '\b':
			if (vga_terminal_column > 0)
			{
				vga_terminal_column--;
			}
			else if (vga_terminal_row > 0)
			{
				vga_terminal_row--;
				vga_terminal_column = VGA_TERMINAL_WIDTH - 1;
			}
			put_entry_at(' ', vga_terminal_color, vga_terminal_column, vga_terminal_row);
			break;

		default:
			put_entry_at(c, vga_terminal_color, vga_terminal_column, vga_terminal_row);
			vga_terminal_column++;
			if (vga_terminal_column >= VGA_TERMINAL_WIDTH)
			{
				vga_terminal_column = 0;
				vga_terminal_row++;
			}
			break;
		}

		if (vga_terminal_row >= VGA_TERMINAL_HEIGHT)
		{
			scroll_terminal();
			vga_terminal_row = VGA_TERMINAL_HEIGHT - 1;
		}
	}

	update_cursor(vga_terminal_column, vga_terminal_row);
}

static void clear(void)
{
	for (uint16_t y = 0; y < VGA_TERMINAL_HEIGHT; y++)
	{
		for (uint16_t x = 0; x < VGA_TERMINAL_WIDTH; x++)
		{
			const uint16_t index = y * VGA_TERMINAL_WIDTH + x;
			vga_terminal_buffer[index] = entry(' ', vga_terminal_color);
		}
	}
	vga_terminal_row = 0;
	vga_terminal_column = 0;
}

void vga_terminal_init(uint32_t buffer_address, vga_terminal_color_enum_t foreground_color, vga_terminal_color_enum_t background_color)
{
	vga_terminal_buffer = (uint16_t *)buffer_address;

	vga_terminal_row = 0;
	vga_terminal_column = 0;
	vga_terminal_color = entry_color(foreground_color, background_color);
	vga_terminal_is_using = true;

	kernel_context->video_state.is_text_mode = true;
	kernel_context->video_state.lfb = DEFAULT_VGA_TERMINAL_BUFFER_ADDRESS;
	kernel_context->video_state.width = VGA_TERMINAL_WIDTH;
	kernel_context->video_state.height = VGA_TERMINAL_HEIGHT;
	kernel_context->video_state.bpp = 0;
	kernel_context->video_state.pitch = 0;

	clear();
}

void vga_terminal_show_cursor(bool show)
{
	if (show)
	{
		outportb(0x3D4, 0x0A);
		outportb(0x3D5, (inportb(0x3D5) & 0xC0) | 14);

		outportb(0x3D4, 0x0B);
		outportb(0x3D5, (inportb(0x3D5) & 0xE0) | 15);
	}
	else
	{
		outportb(0x3D4, 0x0A);
		outportb(0x3D5, 0x20);
	}
}

void vga_terminal_destroy(void)
{
	vga_terminal_row = 0;
	vga_terminal_column = 0;

	clear();

	update_cursor(vga_terminal_row, vga_terminal_column);
	vga_terminal_show_cursor(false);

	vga_terminal_is_using = false;
}

// wrapper
void vga_terminal_clear(void){
	clear();
}

void vga_terminal_write_string(const char *format, ...)
{
	if (!vga_terminal_buffer || !vga_terminal_is_using)
		return;

	char buffer[512];
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
			vga_terminal_write(buffer, strlen(buffer));
			buffer_index = 0;
		}
	}

	buffer[buffer_index] = '\0';
	vga_terminal_write(buffer, strlen(buffer));

	va_end(args);
}