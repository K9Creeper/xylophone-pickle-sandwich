/// -----------
/// @file pci.h

#ifndef _PCI_H
#define _PCI_H

#include <memory.h>

#include <kernel/data-structures/drivers/pci.h>

typedef struct pci_driver_s
{
    uint16_t vendor_id;
    uint16_t device_id;

    uint8_t class_code;
    uint8_t subclass;

    const char *name;

    uint8_t (*probe)(struct pci_device_s *dev);
    void (*remove)(struct pci_device_s *dev);
} pci_driver_t;

static inline uint32_t pci_make_address(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset)
{
    return 0x80000000 |
           ((uint32_t)bus  << 16) |
           ((uint32_t)(slot & 0x1F) << 11) |
           ((uint32_t)(func & 0x07) << 8) |
           (offset & 0xFC);
}

static inline uint32_t pci_read32(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset)
{
    outportl(PCI_CONFIG_ADDRESS,
             pci_make_address(bus, slot, func, offset));
    return inportl(PCI_CONFIG_DATA);
}

static inline uint16_t pci_read16(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset)
{
    uint32_t value = pci_read32(bus, slot, func, offset);
    return (value >> ((offset & 2) * 8)) & 0xFFFF;
}

static inline uint8_t pci_read8(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset)
{
    uint32_t value = pci_read32(bus, slot, func, offset);
    return (value >> ((offset & 3) * 8)) & 0xFF;
}

static inline void pci_write32(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset,
    uint32_t data)
{
    outportl(PCI_CONFIG_ADDRESS,
             pci_make_address(bus, slot, func, offset));
    outportl(PCI_CONFIG_DATA, data);
}

static inline void pci_write16(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset,
    uint16_t data)
{
    uint32_t value = pci_read32(bus, slot, func, offset);
    uint32_t shift = (offset & 2) * 8;

    value &= ~(0xFFFFu << shift);
    value |=  ((uint32_t)data << shift);

    pci_write32(bus, slot, func, offset, value);
}

static inline void pci_write8(
    uint8_t bus,
    uint8_t slot,
    uint8_t func,
    uint8_t offset,
    uint8_t data)
{
    uint32_t value = pci_read32(bus, slot, func, offset);
    uint32_t shift = (offset & 3) * 8;

    value &= ~(0xFFu << shift);
    value |=  ((uint32_t)data << shift);

    pci_write32(bus, slot, func, offset, value);
}

#endif