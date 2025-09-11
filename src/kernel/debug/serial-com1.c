/// --------------------
/// @file serial-com1.c

#include "serial-com1.h"

#include <stdint.h>
#include <stdarg.h>

#include <memory.h>
#include <string.h>

#define COM1_PORT_ADDRESS 0x3F8
#define BUFFER_SIZE 512

static uint8_t is_initialized = 0;

static inline void serial_write(char c)
{
    while ((inportb(COM1_PORT_ADDRESS + 5) & 0x20) == 0)
    {
        // wait for the transmit buffer to be empty
    }
    outportb(COM1_PORT_ADDRESS, c); // write to the serial port
}

static inline void serial_print(const char *str)
{
    while (*str)
    {
        serial_write(*str++);
    }
}

void kernel_serial_com1_init(void)
{
    if(is_initialized)
        return;
        
    outportb(COM1_PORT_ADDRESS + 1, 0x00); 
    outportb(COM1_PORT_ADDRESS + 3, 0x80); 
    outportb(COM1_PORT_ADDRESS + 0, 0x03);
    outportb(COM1_PORT_ADDRESS + 1, 0x00);
    outportb(COM1_PORT_ADDRESS + 3, 0x03); 
    outportb(COM1_PORT_ADDRESS + 2, 0xC7); 
    outportb(COM1_PORT_ADDRESS + 4, 0x0B);

    is_initialized = 1;
}

static inline void append_to_buffer(char *buffer, int *index, const char *src) {
    while (*src) buffer[(*index)++] = *src++;
}

static void ftoa(float val, char *buf, int precision) {
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }

    int int_part = (int)val;
    float frac_part = val - int_part;

    itoa(int_part, buf, 10);
    while (*buf) buf++;  // Move pointer to end of int part string

    *buf++ = '.';  // decimal point

    // Convert fractional part
    for (int i = 0; i < precision; i++) {
        frac_part *= 10;
    }

    int frac_int = (int)(frac_part + 0.5f);  // rounding

    // Convert fractional part with leading zeros if needed
    char frac_buf[16];
    uitoa(frac_int, frac_buf, 10);

    // Add leading zeros if frac_int has fewer digits than precision
    int frac_len = 0;
    while (frac_buf[frac_len]) frac_len++;

    for (int i = frac_len; i < precision; i++) {
        *buf++ = '0';
    }

    for (int i = 0; i < frac_len; i++) {
        *buf++ = frac_buf[i];
    }

    *buf = '\0';
}

void dbgprintf(const char *format, ...) {
    if (!is_initialized) return;

    char buffer[BUFFER_SIZE];
    int buffer_index = 0;
    va_list args;
    va_start(args, format);

    for (const char *ptr = format; *ptr; ++ptr) {
        if (*ptr == '%') {
            ++ptr;

            // Optional width/zero-padding
            int width = 0;
            uint8_t zero_pad = 0;
            if (*ptr == '0') {
                zero_pad = 1;
                ++ptr;
            }
            while (*ptr >= '0' && *ptr <= '9') {
                width = width * 10 + (*ptr - '0');
                ++ptr;
            }

            char temp[64] = {0};
            char pad_char = zero_pad ? '0' : ' ';

            switch (*ptr) {
                case 'd':
                case 'i':
                    itoa(va_arg(args, int), temp, 10);
                    break;
                case 'D':
                case 'u':
                    uitoa(va_arg(args, uint32_t), temp, 10);
                    break;
                case 'x':
                    itoa(va_arg(args, int), temp, 16);
                    break;
                case 'X':
                    uitoa(va_arg(args, uint32_t), temp, 16);
                    break;
                case 'p': {
                    uintptr_t val = (uintptr_t)va_arg(args, void *);
                    temp[0] = '0';
                    temp[1] = 'x';
                    uitoa((uint32_t)val, temp + 2, 16);
                    break;
                }
                case 'c':
                    temp[0] = (char)va_arg(args, int);
                    temp[1] = '\0';
                    break;
                case 's':
                    append_to_buffer(buffer, &buffer_index, va_arg(args, const char *));
                    continue; // skip width padding for strings
                case 'f': {
                     double val = va_arg(args, double);
                    ftoa((float)val, temp, 6);  // 6 decimal places
                    break;
                }
                case '%':
                    temp[0] = '%';
                    temp[1] = '\0';
                    break;
                default:
                    temp[0] = '%';
                    temp[1] = *ptr;
                    temp[2] = '\0';
                    break;
            }

            // Apply width/zero-padding
            int len = 0;
            while (temp[len]) ++len;
            for (int i = len; i < width; ++i) buffer[buffer_index++] = pad_char;

            append_to_buffer(buffer, &buffer_index, temp);
        } else {
            buffer[buffer_index++] = *ptr;
        }

        // flush buffer if nearly full
        if (buffer_index >= BUFFER_SIZE - 64) {
            buffer[buffer_index] = '\0';
            serial_print(buffer);
            buffer_index = 0;
        }
    }

    buffer[buffer_index] = '\0';
    serial_print(buffer);
    va_end(args);
}