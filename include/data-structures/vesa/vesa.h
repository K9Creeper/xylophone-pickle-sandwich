/// ------
/// vesa.h

#ifndef VESA_H
#define VESA_H

#include <stdint.h>

typedef struct vbe_info_block_s
{
    char signature[4];     // must be "VESA" to indicate valid VBE support
    uint16_t version;      // VBE version; high byte is major version, low byte is minor version
    uint32_t oem;          // segment:offset pointer to OEM
    uint32_t capabilities; // bitfield that describes card capabilities
    uint32_t videoModes;   // segment:offset pointer to list of supported video modes
    uint16_t videoMemory;  // amount of video memory in 64KB blocks
    uint16_t softwareRev;  // software revision
    uint32_t vendor;       // segment:offset to card vendor string
    uint32_t productName;  // segment:offset to card model name
    uint32_t productRev;   // segment:offset pointer to product revision
    char reserved[222];    // reserved for future expansion
    char oemData[256];     // OEM BIOSes store their strings in this area
}
__attribute__((packed)) vbe_info_block_t;

typedef struct vbe_mode_info_s
{
    uint16_t attributes;
    uint8_t windowA, windowB;
    uint16_t granularity;
    uint16_t windowSize;
    uint16_t segmentA, segmentB;
    uint32_t winFuncPtr;
    uint16_t pitch;

    uint16_t width, height;
    uint8_t wChar, yChar, planes, bpp, banks;
    uint8_t memoryModel, bankSize, imagePages;
    uint8_t reserved0;

    uint8_t readMask, redPosition;
    uint8_t greenMask, greenPosition;
    uint8_t blueMask, bluePosition;
    uint8_t reservedMask, reservedPosition;
    uint8_t directColorAttributes;

    // linear frame buffer
    uint32_t physbase;
    uint32_t offScreenMemOff;
    uint16_t offScreenMemSize;
    uint8_t reserved1[206];
} __attribute__((packed)) vbe_mode_info_t;

typedef struct vesa_mode_s
{
    uint32_t number;
    vbe_mode_info_t info;
}vesa_mode_t;

#endif