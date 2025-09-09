/// ----------------
/// @file syscalls.h

#ifndef SYSCALLS_H
#define SYSCALLS_H

#define SYSCALLS_MAX_COUNT 1024

#ifndef SYSCALLS_ENUM
#define SYSCALLS_ENUM
typedef enum syscalls_e
{
    SYSCALLS_TICK,
    SYSCALLS_EXIT,
    SYSCALLS_SLEEP,

    SYSCALLS_PID,

    SYSCALLS_MALLOC,
    SYSCALLS_FREE
} syscalls_t;
#endif

typedef struct syscall_entry_s
{
    syscalls_t num;
    void *func;
} syscall_entry_t;

#define REGISTER_SYSCALL(num, func)               \
    static const syscall_entry_t __syscall_##func \
        __attribute__((used, section(".syscalls"))) = {num, func};

#endif