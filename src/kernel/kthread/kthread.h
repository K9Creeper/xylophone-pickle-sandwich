/// ---------
/// kthread.h
/// @brief This file declares the functions for handling kernel threads.

#ifndef KTHREAD_H
#define KTHREAD_H

#include <data-structures/kernel/kthread.h>

extern int kthread_start(char* name, int argc, char* argv[]);
extern int kthread_register(kthread_entry_t entry, char* name);

#endif