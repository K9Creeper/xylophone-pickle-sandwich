/// ------
/// vesa.h

#ifndef VESA_H
#define VESA_H

#include <stdint.h>

typedef enum vbe_return_status_e {
    // AL: function support
    VBE_FUNCTION_SUPPORTED       = 0x4F,  

    // AH: function return status
    VBE_SUCCESS                  = 0x00,  // Function call successful
    VBE_FAILED                   = 0x01,  // Function call failed
    VBE_NOT_SUPPORTED_BY_HARDWARE = 0x02, // Function not supported by hardware
    VBE_INVALID_IN_CURR_MODE      = 0x03  // Function invalid in current mode
} vbe_return_status_t;

#define VBE_AL(ax) ((uint8_t)((ax) & 0x00FF))
#define VBE_AH(ax) ((uint8_t)(((ax) >> 8) & 0x00FF))

#define VBE_IS_FUNCTION_SUPPORTED(ax) (VBE_AL(ax) == VBE_FUNCTION_SUPPORTED)
#define VBE_STATUS_SUCCESS(ax)           (VBE_AH(ax) == VBE_SUCCESS)
#define VBE_STATUS_FAILED(ax)            (VBE_AH(ax) == VBE_FAILED)
#define VBE_STATUS_NOT_SUPPORTED(ax)     (VBE_AH(ax) == VBE_NOT_SUPPORTED_BY_HARDWARE)
#define VBE_STATUS_INVALID_IN_MODE(ax)   (VBE_AH(ax) == VBE_INVALID_IN_CURR_MODE)

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