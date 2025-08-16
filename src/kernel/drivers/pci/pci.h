/// -----
/// pci.h
/// @brief This file

#ifndef DRIVERS_PCI_H
#define DRIVERS_PCI_H

#include <data-structures/drivers/pci.h>

extern void pci_init(void);

extern pci_device_t* pci_get_devices(uint32_t* device_count);

extern const char* pci_get_class_name(pci_device_t* dev);
extern const char* pci_get_device_name(pci_device_t* dev);
extern const char* pci_get_vendor_name(pci_device_t* dev);

#endif