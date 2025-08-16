/// -----
/// ata.h
/// @brief This file declares the functions relating to ata.

#ifndef DRIVERS_ATA_H
#define DRIVERS_ATA_H

#include <data-structures/drivers/ata.h>
#include <data-structures/drivers/pci.h>

extern void ata_init(pci_device_t* dev);

#endif