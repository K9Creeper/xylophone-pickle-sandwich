/// -----
/// pci.c
/// @brief This

#include "pci.h"

#include <stddef.h>
#include <memory.h>

#include "../ata/ata.h"

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

static pci_device_t pci_devices[MAX_PCI_DEVICES] = {0};
static uint32_t pci_devices_count = 0;

static pci_driver_t registered_drivers[] = {
    {0x8086, 0x7010, (pci_driver_attach_handle_t)ata_init}, // PIIX3 IDE
    {0x8086, 0x7111, (pci_driver_attach_handle_t)ata_init}, // PIIX4 IDE
    {0, 0, 0}};

static uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    address = (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    outportl(0xCF8, address);
    tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

static void pci_write_dword(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint32_t data)
{
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
                         (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    outportl(0xCF8, address);
    outportl(0xCFC, data);
}

inline static uint16_t pci_get_device_class(uint16_t bus, uint16_t slot, uint16_t function)
{
    return (pci_read_word(bus, slot, function, 0xA) & 0xFF00) >> 8;
}

inline static uint8_t pci_get_device_irq(uint16_t bus, uint16_t slot, uint16_t func)
{
    return (uint8_t)(pci_read_word(bus, slot, func, 0x3C) & 0x00ff);
}

inline static uint32_t pci_get_device_base32(uint16_t bus, uint16_t slot, uint16_t function)
{
    return (uint32_t)pci_read_word(bus, slot, function, 0x12) << 16 | (uint32_t)pci_read_word(bus, slot, function, 0x10);
}

void pci_enable_device_busmaster(uint16_t bus, uint16_t slot, uint16_t function)
{
    uint16_t dev_command_reg = pci_read_word(bus, slot, function, 0x04);
    uint16_t dev_status_reg = pci_read_word(bus, slot, function, 0x06);
    dev_command_reg |= (1 << 2);
    pci_write_dword(bus, slot, function, 0x04, (uint32_t)dev_status_reg << 16 | (uint32_t)dev_command_reg);
}

int pci_register_device(uint32_t bus, uint32_t slot, uint32_t function, uint16_t vendor, uint16_t device, uint16_t class, uint8_t irq, uint32_t base)
{
    if (pci_devices_count > MAX_PCI_DEVICES)
        return -1;

    pci_device_t pci_dev;

    pci_dev.device = device;
    pci_dev.vendor = vendor;
    pci_dev.bus = bus;
    pci_dev.slot = slot;
    pci_dev.function = function;
    pci_dev.class = class;
    pci_dev.irq = irq;
    pci_dev.base = base;

    pci_devices[pci_devices_count] = pci_dev;
    pci_devices_count++;

    return pci_devices_count - 1;
}

pci_device_t *pci_get_devices(uint32_t *device_count)
{
    if (device_count != NULL)
        *device_count = pci_devices_count;

    return pci_devices;
}

const char *pci_get_class_name(pci_device_t *dev)
{
    return pci_classes[dev->class];
}

const char *pci_get_device_name(pci_device_t *dev)
{
    return "Unknown";
}

const char *pci_get_vendor_name(pci_device_t *dev)
{
    return "Unknown";
}

#include <stdio.h>
void pci_init(void)
{
    int devices_found = 0;
    for (uint32_t bus = 0; bus < 256; bus++)
    {
        for (uint32_t slot = 0; slot < 32; slot++)
        {
            for (uint32_t function = 0; function < 8; function++)
            {
                uint16_t vendor = pci_read_word(bus, slot, function, 0);
                if (vendor == 0xffff)
                    continue;

                uint16_t device = pci_read_word(bus, slot, function, 2);
                uint16_t class = pci_get_device_class(bus, slot, function);
                uint8_t irq = pci_get_device_irq(bus, slot, function);
                uint32_t base = pci_get_device_base32(bus, slot, function);

                int driver_index = pci_register_device(bus, slot, function, vendor, device, class, irq, base);

                printf("PCI Device 0x%X, 0x%X, %s\n", vendor, device, pci_get_class_name(&pci_devices[driver_index]));

                devices_found++;

                int i = 0;
                for (pci_driver_t driver = registered_drivers[i]; driver.vendor != 0; driver = registered_drivers[i])
                {
                    if (driver.vendor == vendor && driver.device == device && driver.attach != NULL)
                    {
                        driver.attach((void*)(&pci_devices[driver_index]));
                    }
                    i++;
                }
            }
        }
    }
}