/// -----------
/// @file pci.c

#include "pci.h"

#include <kernel/dbgprintf.h>

static pci_device_t pci_devices[PCI_MAX_DEVICES];
static uint8_t pci_scanned_bus[256] = {0};
static uint8_t pci_decoded[256][32][8] = {0};
static uint32_t pci_device_count = 0;

static void pci_read_config_block(uint8_t bus, uint8_t slot, uint8_t func, void *buffer, uint8_t size)
{
    for (uint8_t offset = 0; offset < size; offset += 4)
    {
        *(uint32_t *)((uint8_t *)buffer + offset) =
            pci_read32(bus, slot, func, offset);
    }
}

static void pci_decode_device(uint8_t bus, uint8_t slot, uint8_t func, pci_device_t *dev)
{
    dev->bus = bus;
    dev->slot = slot;
    dev->function = func;

    uint8_t header_type =
        pci_read8(bus, slot, func, 0x0E) & 0x7F;

    dev->header_type = (pci_header_type_t)header_type;

    switch (header_type)
    {
    case pci_header_type_general:
        pci_read_config_block(
            bus, slot, func,
            &dev->header.type0,
            sizeof(pci_header_type0_t));
        break;

    case pci_header_type_pci_to_pci:
        pci_read_config_block(
            bus, slot, func,
            &dev->header.type1,
            sizeof(pci_header_type1_t));
        break;

    case pci_header_type_pci_to_card:
        pci_read_config_block(
            bus, slot, func,
            &dev->header.type2,
            sizeof(pci_header_type2_t));
        break;

    default:
        return;
    }

    pci_decoded[bus][slot][func] = 1;
}

static void pci_register_device(
    uint8_t bus, uint8_t slot, uint8_t func)
{
    if (pci_device_count >= PCI_MAX_DEVICES)
        return;

    if (pci_decoded[bus][slot][func])
        return;

    pci_device_t *dev = &pci_devices[pci_device_count++];
    pci_decode_device(bus, slot, func, dev);

    const pci_device_common_t *c =
        &dev->header.type0.common;

    if (dev->header_type == pci_header_type_pci_to_pci)
    {
        uint8_t sec = dev->header.type1.secondary_bus;
        uint8_t sub = dev->header.type1.subordinate_bus;

        for (uint8_t b = sec; b <= sub; b++)
        {
            pci_scan_bus(b);
        }
    }
}

void pci_scan_bus(uint8_t bus)
{
    if (pci_scanned_bus[bus])
        return;

    pci_scanned_bus[bus] = 1;

    for (uint8_t slot = 0; slot < 32; slot++)
    {
        uint16_t vendor = pci_read16(bus, slot, 0, 0x00);
        if (vendor == 0xFFFF)
            continue;

        uint8_t header_type =
            pci_read8(bus, slot, 0, 0x0E);

        uint8_t functions = (header_type & 0x80) ? 8 : 1;

        for (uint8_t func = 0; func < functions; func++)
        {
            vendor = pci_read16(bus, slot, func, 0x00);
            if (vendor == 0xFFFF)
                continue;

            pci_register_device(bus, slot, func);
        }
    }
}

const pci_device_t* pci_get_devices(uint32_t* count){
    if(count) *count = pci_device_count;
    return pci_devices;
}
