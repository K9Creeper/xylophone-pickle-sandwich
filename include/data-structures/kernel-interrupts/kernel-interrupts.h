/// -------------------
/// kernel-interrupts.h
/// @brief This file defines the typedefs for kernel interrupt handles.

#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H

#include <registers.h>

typedef void(*kernel_interrupt_request_handle_t)(registers_t* regs);
typedef void (*kernel_interrupt_service_handle_t)(registers_t *regs);
typedef void (*kernel_interrupt_service_fault_handle_t)(registers_t *regs);

#endif