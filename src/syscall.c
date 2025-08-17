/// ---------
/// syscall.h
/// @brief This file defines the function syscall.

#include <syscall.h>

#include <stdio.h>

static inline int _syscall(syscall_t i, int arg1, int arg2, int arg3)
{
    int ret;
    __asm__ volatile (
        "int $0x80"
        : "=a" (ret)                  /* output: ret in eax */
        : "a" (i),                    /* input: syscall number in eax */
          "b" (arg1),                 /* input: arg1 in ebx */
          "c" (arg2),                 /* input: arg2 in ecx */
          "d" (arg3)                  /* input: arg3 in edx */
        : "memory"                    /* clobbers */
    );
    return ret;
}

void yield(void){
    _syscall(SYSCALL_YIELD, 0, 0, 0);
}

void exit(void){
    _syscall(SYSCALL_EXIT, 0, 0, 0);
}

void sleep(int ms){
    _syscall(SYSCALL_SLEEP, ms, 0, 0);
}

void* malloc(int size){
    return (void*)_syscall(SYSCALL_MALLOC, size, 0, 0);
}

void free(void* address){
    _syscall(SYSCALL_FREE, (uint32_t)address, 0, 0);
}

uint32_t get_system_tick_count(void){
    _syscall(SYSCALL_GET_SYSTEM_TICK_COUNT, 0, 0, 0);
}

void get_task_directory(char* buffer, uint32_t buffer_size){
    _syscall(SYSCALL_GET_TASK_DIRECTORY, (uint32_t)buffer, buffer_size, 0);
}

void* open_file(const char* path){
    return (void*)_syscall(SYSCALL_OPEN, (uint32_t)path, 0, 0);
}

uint32_t read_file(void* file, void* buffer, uint32_t buffer_size){
    return (uint32_t)_syscall(SYSCALL_READ, (uint32_t)file, (uint32_t)buffer, buffer_size);
}

void* list_dir(const char* path, uint32_t* max_size){
    return (void*)_syscall(SYSCALL_LIST_DIR, (uint32_t)path, (uint32_t)max_size, 0);
}