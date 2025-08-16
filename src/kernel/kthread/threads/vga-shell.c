/// -----------
/// vga-shell.c
/// @brief This file defines the vga-shell kthread.

#include <data-structures/kernel/kernel-context.h>

#include <memory.h>
#include <syscall.h>
#include <kernel/util.h>

#include "../../drivers/vga-terminal/vga-terminal.h"

// Still getting input from the keyboard handle...
#include "../../../terminal/terminal.h"

#include <stdio.h>

void kthread_vga_shell(void)
{
    vga_terminal_clear();

    char buffer[512];
    char dir[512];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(dir, 0, sizeof(dir));
        get_task_directory(dir, sizeof(dir));
        vga_terminal_write_string("%s> ", dir);
        while (!terminal_get_input(buffer, sizeof(buffer)))
        {
            __asm__ volatile("hlt");
        }

        vga_terminal_write_string("You said: %s\n", buffer);
        // Do something with input..
    }
    exit();
}