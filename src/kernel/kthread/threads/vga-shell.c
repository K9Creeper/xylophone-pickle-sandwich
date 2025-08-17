/// -----------
/// vga-shell.c
/// @brief This file defines the vga-shell kthread.

#include <data-structures/kernel/kernel-context.h>

#include <memory.h>
#include <string.h>
#include <syscall.h>
#include <kernel/util.h>

#include "../../drivers/vga-terminal/vga-terminal.h"

// still getting input from the keyboard handle...
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

        if(strcmp(buffer, "ls") == 0){
            uint32_t count = 512;
            file_info_t* FILES = list_dir(dir, &count);

            if(FILES){
                for(int i = 0; i < count; i++){
                    vga_terminal_write_string("%s ", FILES[i].name);
                }
                vga_terminal_write_string("\n");
                free(FILES);
            }
        }else if(strcmp(buffer, "test") == 0){
            file_info_t* FILE = open_file("BOOT/../BOOT/./../BOOT/kernel.bin");

            if(FILE){
                vga_terminal_write_string("%s\n", FILE->name);
                free(FILE);
            }
        }

        // Do something with input..
    }
    exit();
}