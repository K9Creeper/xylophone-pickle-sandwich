/// -----------
/// @file pci.c

#include "pci.h"

#include <kernel/dbgprintf.h>

static const char *pci_classes[] =
    {
        [0x0] = "Unknown",
        [0x1] = "Storage controller",
        [0x2] = "Network controller",
        [0x3] = "Display controller",
        [0x4] = "Multimedia device",
        [0x5] = "Memory controller",
        [0x6] = "Bridge device",
        [0x7] = "Simple Communication Controller",
        [0x8] = "Base System Peripheral",
        [0x9] = "Input Device Controller",
        [0xA] = "Docking Station",
        [0xB] = "Processor",
        [0xC] = "Serial Bus Controller",
        [0xD] = "Wireless Controller",
        [0xE] = "Intelligent Controller",
        [0xF] = "Satellite Communication Controller",
        [0x10] = "Encryption Controller",
        [0x11] = "Signal Processing Controller"
};


void pci_scan_bus(uint8_t bus) {
    for (uint8_t slot = 0; slot < 32; slot++) {
        uint16_t vendor = pci_read16(bus, slot, 0, 0x00);
        if (vendor == 0xFFFF)
            continue;

        uint8_t header_type =
            pci_read8(bus, slot, 0, 0x0E);

        uint8_t functions = (header_type & 0x80) ? 8 : 1;

        for (uint8_t func = 0; func < functions; func++) {
            vendor = pci_read16(bus, slot, func, 0x00);
            if (vendor == 0xFFFF)
                continue;

            uint16_t device = pci_read16(bus, slot, func, 2);
            uint16_t class = (pci_read16(bus, slot, func, 0xA) & 0xFF00) >> 8;
            
            dbgprintf("PCI Device 0x%X, 0x%X, %s\n", vendor, device, pci_classes[class]);
        }
    }
}
