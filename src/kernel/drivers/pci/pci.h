/// -----------
/// @file pci.h

#ifndef DRIVERS_PCI_H
#define DRIVERS_PCI_H

#include <kernel/data-structures/drivers/pci.h>
#include <kernel/drivers/pci.h>

extern void pci_scan_bus(uint8_t bus);

#endif