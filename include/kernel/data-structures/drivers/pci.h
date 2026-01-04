/// -----------
/// @file pci.h

#ifndef PCI_H
#define PCI_H

#include <stdint.h>

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

typedef enum pci_header_type_e
{
    pci_header_type_general = 0x0,    /* Type 0 */
    pci_header_type_pci_to_pci = 0x1, /* Type 1 */
    pci_header_type_pci_to_card = 0x2 /* Type 2 (CardBus) */
} pci_header_type_t;

typedef struct pci_device_common_s
{
    uint16_t vendor_id; /* 0x00 */
    uint16_t device_id; /* 0x02 */

    uint16_t command; /* 0x04 */
    uint16_t status;  /* 0x06 */

    uint8_t rev_id;        /* 0x08 */
    uint8_t prog_if;       /* 0x09 */
    uint8_t subclass_code; /* 0x0A */
    uint8_t class_code;    /* 0x0B */

    uint8_t cache_line_size; /* 0x0C */
    uint8_t latency_timer;   /* 0x0D */
    uint8_t type;            /* 0x0E */
    uint8_t BIST;            /* 0x0F */
} pci_device_common_t;

typedef struct pci_header_type0_s
{
    pci_device_common_t common;

    uint32_t BAR[6]; /* 0x10–0x27 */

    uint32_t cardbus_cis_ptr; /* 0x28 */

    uint16_t subsystem_vendor_id; /* 0x2C */
    uint16_t subsystem_id;        /* 0x2E */

    uint32_t expansion_rom_base; /* 0x30 */

    uint8_t capabilities_ptr; /* 0x34 */
    uint8_t reserved0[3];

    uint32_t reserved1; /* 0x38 */

    uint8_t interrupt_line; /* 0x3C */
    uint8_t interrupt_pin;  /* 0x3D */
    uint8_t min_grant;      /* 0x3E */
    uint8_t max_latency;    /* 0x3F */
} pci_header_type0_t;

typedef struct pci_header_type1_s
{
    pci_device_common_t common;

    uint32_t BAR[2]; /* 0x10–0x17 */

    uint8_t primary_bus;             /* 0x18 */
    uint8_t secondary_bus;           /* 0x19 */
    uint8_t subordinate_bus;         /* 0x1A */
    uint8_t secondary_latency_timer; /* 0x1B */

    uint8_t io_base;           /* 0x1C */
    uint8_t io_limit;          /* 0x1D */
    uint16_t secondary_status; /* 0x1E */

    uint16_t mem_base;  /* 0x20 */
    uint16_t mem_limit; /* 0x22 */

    uint16_t prefetch_base;  /* 0x24 */
    uint16_t prefetch_limit; /* 0x26 */

    uint32_t prefetch_base_upper;  /* 0x28 */
    uint32_t prefetch_limit_upper; /* 0x2C */

    uint16_t io_base_upper;  /* 0x30 */
    uint16_t io_limit_upper; /* 0x32 */

    uint8_t capabilities_ptr; /* 0x34 */
    uint8_t reserved0[3];

    uint32_t expansion_rom_base; /* 0x38 */

    uint8_t interrupt_line;  /* 0x3C */
    uint8_t interrupt_pin;   /* 0x3D */
    uint16_t bridge_control; /* 0x3E */
} pci_header_type1_t;

typedef struct pci_header_type2_s
{
    pci_device_common_t common;

    uint32_t cardbus_socket_base; /* 0x10 */

    uint8_t capabilities_ptr; /* 0x14 */
    uint8_t reserved0;
    uint16_t secondary_status; /* 0x16 */

    uint8_t pci_bus;         /* 0x18 */
    uint8_t cardbus_bus;     /* 0x19 */
    uint8_t subordinate_bus; /* 0x1A */
    uint8_t cardbus_latency; /* 0x1B */

    uint32_t mem_base0;  /* 0x1C */
    uint32_t mem_limit0; /* 0x20 */
    uint32_t mem_base1;  /* 0x24 */
    uint32_t mem_limit1; /* 0x28 */

    uint32_t io_base0;  /* 0x2C */
    uint32_t io_limit0; /* 0x30 */
    uint32_t io_base1;  /* 0x34 */
    uint32_t io_limit1; /* 0x38 */

    uint8_t interrupt_line;  /* 0x3C */
    uint8_t interrupt_pin;   /* 0x3D */
    uint16_t bridge_control; /* 0x3E */
} pci_header_type2_t;

#endif