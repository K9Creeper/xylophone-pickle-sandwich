/// ---------------
/// @file syscall.h


#ifndef SYSCALLS_LIB_H
#define SYSCALLS_LIB_H

#include <stdint.h>

#ifndef SYSCALLS_ENUM
#define SYSCALLS_ENUM
typedef enum syscalls_e
{
    SYSCALLS_TICK,
    SYSCALLS_EXIT,
    SYSCALLS_SLEEP,
    
    SYSCALLS_MALLOC,
    SYSCALLS_FREE
} syscalls_t;
#endif


static inline int _syscall(syscalls_t i, int arg1, int arg2, int arg3)
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

// no arguments
#define SYSCALL_0(n)            _syscall((n), 0, 0, 0)

// 1 argument
#define SYSCALL_1(n, a1)        _syscall((n), (int)(a1), 0, 0)

// 2 arguments
#define SYSCALL_2(n, a1, a2)    _syscall((n), (int)(a1), (int)(a2), 0)

// 3 arguments
#define SYSCALL_3(n, a1, a2, a3) _syscall((n), (int)(a1), (int)(a2), (int)(a3))

static inline uint32_t get_system_tick_count(void){
    return SYSCALL_0(SYSCALLS_TICK);
}

static inline void exit(void){
    SYSCALL_0(SYSCALLS_EXIT);
}

static inline void sleep(uint32_t ms){
    SYSCALL_1(SYSCALLS_SLEEP, (int)ms);
}

static inline void* malloc(int size){
    return (void*)SYSCALL_1(SYSCALLS_MALLOC, size);
}

static inline void free(void* address){
    SYSCALL_1(SYSCALLS_FREE, address);
}

#endif