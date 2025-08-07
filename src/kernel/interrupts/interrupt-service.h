/// -------------------
/// interrupt-service.h
/// @brief This file declares the needed functions to add handles to external (device) interrupts.

#ifndef KERNEL_INTERRUPT_SERVICE_H
#define KERNEL_INTERRUPT_SERVICE_H

#define KERNEL_INTERRUPT_SERVICE_MAX_HANNDLE_COUNT 256
#define KERNEL_INTERRUPT_SERVICE_FAULT_MAX_HANNDLE_COUNT 32

#include <stdint.h>

#include <registers.h>
#include <data-structures/kernel-interrupts/kernel-interrupts.h>

extern void kernel_interrupt_service_set_handle(uint16_t idx, kernel_interrupt_service_handle_t handle);
extern void kernel_interrupt_service_remove_handle(uint16_t idx);

extern void kernel_interrupt_service_set_fault_handle(uint16_t idx, kernel_interrupt_service_fault_handle_t handle);
extern void kernel_interrupt_service_remove_fault_handle(uint16_t idx);

#endif