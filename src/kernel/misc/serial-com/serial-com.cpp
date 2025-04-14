/// ----------
/// serial-com.cpp
/// @brief This file defines the needed function(s) to setup the COM1 serial port,
/// aswell as debuging through said port.

#include "serial-com.hpp"

#include <stdarg.h>

#include "../../../helpers/memory.h"
#include "../../../helpers/string.h"

static bool bInitialized = false;

static void serial_write(char c)
{
    while ((inportb(COM1_PORT_ADDRESS + 5) & 0x20) == 0)
    {
        // wait for the transmit buffer to be empty
    }
    outportb(COM1_PORT_ADDRESS, c); // write to the serial port
}

static void serial_print(const char *str)
{
    while (*str)
    {
        serial_write(*str++);
    }
}

void Kernel::Misc::Serial_Com_Init()
{
    if(bInitialized)
        return;
        
    outportb(COM1_PORT_ADDRESS + 1, 0x00); 
    outportb(COM1_PORT_ADDRESS + 3, 0x80); 
    outportb(COM1_PORT_ADDRESS + 0, 0x03);
    outportb(COM1_PORT_ADDRESS + 1, 0x00);
    outportb(COM1_PORT_ADDRESS + 3, 0x03); 
    outportb(COM1_PORT_ADDRESS + 2, 0xC7); 
    outportb(COM1_PORT_ADDRESS + 4, 0x0B);

    bInitialized = true;
}

extern "C" void printf(const char* format, ...)
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
            serial_print(buffer);
            buffer_index = 0;
        }
    }

    buffer[buffer_index] = '\0';
    serial_print(buffer);

    va_end(args);
}