/// ---------
/// syscall.h
/// @brief This file defines the function syscall.

#include <syscall.h>

static int _syscall(syscall_t i, int arg1, int arg2, int arg3)
{
    int ret;

    __asm__ volatile ("int $0x80"
                  : "=a" (ret)
                  : "%0" (i), "b" (arg1), "c" (arg2), "d" (arg3));
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