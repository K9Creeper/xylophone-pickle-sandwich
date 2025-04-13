/// ----------
/// serial.cpp
/// @brief This file defines the needed function(s) to setup the COM1 serial port,
/// aswell as debuging through said port.

#include "serial.hpp"

#include <stdarg.h>

#include "../../helpers/memory.h"
#include "../../helpers/string.h"

static bool bInitialized = false;

static void serial_write(char c)
{
    while ((inportb(0x3F8 + 5) & 0x20) == 0)
    {
        // Wait for the transmit buffer to be empty
    }
    outportb(0x3F8, c); // Write the character to the serial port
}

static void serial_print(const char *str)
{
    while (*str)
    {
        serial_write(*str++);
    }
}

void Kernel::Misc::COM1_Init()
{
    // Initialize the serial port (COM1 for example)
    outportb(0x3F8 + 1, 0x00); // Disable interrupts
    outportb(0x3F8 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outportb(0x3F8 + 0, 0x03); // Set baud rate to 38400
    outportb(0x3F8 + 1, 0x00); // Disable DLAB
    outportb(0x3F8 + 3, 0x03); // 8 bits, no parity, one stop bit
    outportb(0x3F8 + 2, 0xC7); // Enable FIFO, clear them
    outportb(0x3F8 + 4, 0x0B); // IRQs enabled

    bInitialized = true;
}

extern "C" void printf(const char* format, ...)
{
    if (!bInitialized)
        return;
    char buffer[256]; // Output buffer
    int buffer_index = 0;

    va_list args; // To handle variable arguments
    va_start(args, format);

    for (const char *ptr = format; *ptr != '\0'; ++ptr)
    {
        if (*ptr == '%')
        {
            ++ptr; // Move past '%'
            switch (*ptr)
            {
            case 'd': // Integer
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
            case 'x': // Hexadecimal
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
            case 'X': // Hexadecimal
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
            case 's': // String
            {
                const char *str = va_arg(args, const char *);
                for (const char *c = str; *c != '\0'; ++c)
                {
                    buffer[buffer_index++] = *c;
                }
                break;
            }
            case '%': // Literal '%'
                buffer[buffer_index++] = '%';
                break;
            default:
                // Unknown specifier, just print it
                buffer[buffer_index++] = '%';
                buffer[buffer_index++] = *ptr;
                break;
            }
        }
        else
        {
            buffer[buffer_index++] = *ptr;
        }

        // Flush the buffer if nearly full
        if (buffer_index >= sizeof(buffer) - 1)
        {
            buffer[buffer_index] = '\0';
            serial_print(buffer);
            buffer_index = 0;
        }
    }

    // Null-terminate and flush the remaining buffer
    buffer[buffer_index] = '\0';
    serial_print(buffer);

    va_end(args);
}