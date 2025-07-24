/// -------------------
/// interrupt-request.h
/// @brief This file declares the needed functions to add handles to external (device) interrupts.

#ifndef KERNEL_INTERRUPT_REQUEST_H
#define KERNEL_INTERRUPT_REQUEST_H

#define KERNEL_INTERRUPT_REQUEST_MAX_HANNDLE_COUNT 16

#include <stdint.h>

#include <registers.h>
#include <data-structures/kernel-interrupts/kernel-interrupts.h>

extern void kernel_interrupt_request_set_handle(uint16_t idx, kernel_interrupt_request_handle_t handle);
extern void kernel_interrupt_request_remove_handle(uint16_t idx);

#endif