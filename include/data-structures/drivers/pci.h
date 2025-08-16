/// -----
/// pci.h
/// @brief This file defines 

#ifndef PCI_H
#define PCI_H

#include <stdint.h>

#define MAX_PCI_DEVICES 64

typedef void (*pci_driver_attach_handle_t)(void* dev);

typedef struct pci_driver_s{
    uint16_t vendor, device;
    pci_driver_attach_handle_t attach;
}pci_driver_t;

typedef struct pci_device_s {
    uint32_t bus;
    uint32_t slot;
    uint32_t function;
    uint16_t device;
    uint16_t vendor;

	uint8_t irq;

    uint16_t class;

	uint32_t base;
    pci_driver_t *driver;
}pci_device_t;

#endif