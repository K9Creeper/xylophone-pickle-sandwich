/// -----
/// fat.h
/// @brief This file defines the fat boot record specfication structures.

#ifndef FAT_H
#define FAT_H

#include <stdint.h>

#define FAT_READ_ONLY 0x01 
#define FAT_HIDDEN 0x02 
#define FAT_SYSTEM 0x04 
#define FAT_VOLUME_ID 0x08 
#define FAT_DIRECTORY 0x10 
#define FAT_ARCHIVE 0x20 
#define FAT_LFN (FAT_READ_ONLY|FAT_HIDDEN|FAT_SYSTEM|FAT_VOLUME_ID)

typedef struct fat_boot_record_s
{
    uint8_t jmp[3];                 // 0
    uint8_t manufacturer[8];        // 3
    uint16_t bytes_per_plock;       // 11
    uint8_t blocks_per_allocation;  // 13
    uint16_t reserved_blocks;       // 14
    uint8_t num_FATs;               // 16
    uint16_t root_dir_entries;      // 17
    uint16_t total_blocks;          // 19
    uint8_t media_descriptor;       // 21
    uint16_t fat_blocks;            // 22
    uint16_t blocks_per_track;      // 24
    uint16_t num_heads;             // 26
    uint32_t hidden_blocks;         // 28
    uint32_t large_sector_count;    // 32
} __attribute__((packed)) fat_boot_record_t;

#endif