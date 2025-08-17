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

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

static int change_directory(char *dir, size_t dir_size, const char *path)
{
    if (!dir || !path || dir_size == 0)
        return -1;

    char temp[512];

    // There are no guards for drive selection...
    size_t drive_len = 2;
    printf("%s\n", path);

    strncpy(temp, dir, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    if (strlen(temp) == drive_len)
        strcat(temp, "/");

    char path_copy[512];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';

    char token[512];
    char *tok = strtok(path_copy, "/", token);

    while (tok != NULL)
    {
        if (strcmp(tok, ".") == 0)
        {
            // Do nothing
        }
        else if (strcmp(tok, "..") == 0)
        {
            size_t len = strlen(temp);

            if (len > drive_len + 1)
            {
                printf("%s %d > %d\n", temp, len, drive_len + 1);
                if (temp[len - 1] == '/' && len > drive_len + 1)
                    len--;

                size_t i = len;
                while (i > drive_len && temp[i - 1] != '/')
                    i--;

                if (i > drive_len)
                    temp[i] = '\0';
                else
                {
                    temp[drive_len] = '/';
                    temp[drive_len + 1] = '\0';
                }
            }
        }
        else
        {
            size_t len = strlen(temp);

            bool directory_exists = false;

            uint32_t count = 512;
            file_info_t *FILES = list_dir(temp, &count);
            if (FILES)
            {
                for (uint32_t i = 0; i < count; i++)
                {
                    if (strcmp(tok, FILES[i].name) == 0 && (FILES[i].attributes & FAT_DIRECTORY))
                    {
                        directory_exists = true;
                        break;
                    }
                }
                free(FILES);
            }

            if (directory_exists == false)
                return -2;

            if (len + 1 + strlen(tok) < sizeof(temp))
            {
                if (temp[len - 1] != '/')
                    strcat(temp, "/");
                strcat(temp, tok);
            }
        }

        tok = strtok(NULL, "/", token);
    }

    memcpy(dir, temp, dir_size - 1);
    dir[dir_size - 1] = '\0';
    return 0;
}

void kthread_vga_shell(void)
{
    vga_terminal_clear();

    char buffer[512];
    char token[512];
    char dir[512];
    get_task_directory(dir, sizeof(dir));

    while (true)
    {
        memset(token, 0, sizeof(token));
        memset(buffer, 0, sizeof(buffer));
        vga_terminal_write_string("%s> ", dir);

        while (!terminal_get_input(buffer, sizeof(buffer)))
        {
            __asm__ volatile("hlt");
        }

        strtok(buffer, " ", token);

        if (strcmp(token, "ls") == 0)
        {
            uint32_t count = 512;
            file_info_t *FILES = list_dir(dir, &count);

            if (FILES)
            {
                for (uint32_t i = 0; i < count; i++)
                {
                    vga_terminal_write_string("%s ", FILES[i].name);
                }
                vga_terminal_write_string("\n");
                free(FILES);
            }
        }
        else if (strcmp(token, "cd") == 0)
        {
            char *path = strtok(NULL, " ", token);
            if (path)
            {
                if (change_directory(dir, sizeof(dir), path) != 0)
                {
                    vga_terminal_write_string("cd: no such directory\n", path);
                }
            }
            else
            {
                vga_terminal_write_string("cd: missing argument\n");
            }
        }else if(strcmp(token, "cls") == 0){
            vga_terminal_clear();
        }
        else
        {
            vga_terminal_write_string("Unknown command: %s\n", token);
        }
    }

    exit();
}