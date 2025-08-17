/// -------
/// fat16.h
/// @brief This file defines the structure to the FAT-16 specification.

#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>
#include <data-structures/filesystem/fat.h>

typedef enum
{
    FAT16_FLAG_READ_ONLY = 1 << 0,    /* Indicates that the file is read-only */
    FAT16_FLAG_HIDDEN = 1 << 1,       /* Indicates a hidden file */
    FAT16_FLAG_SYSTEM = 1 << 2,       /* Indicates a system file */
    FAT16_FLAG_VOLUME_LABEL = 1 << 3, /* Indicates a special entry containing the volume label */
    FAT16_FLAG_SUBDIRECTORY = 1 << 4, /* Indicates that the entry describes a subdirectory */
    FAT16_FLAG_ARCHIVE = 1 << 5,      /* Archive flag */
    FAT16_FLAG_UNUSED1 = 1 << 6,      /* Not used; must be set to 0 */
    FAT16_FLAG_UNUSED2 = 1 << 7       /* Not used; must be set to 0 */
} fat16_flag_t;

typedef struct fat16_boot_record_s
{
    fat_boot_record_t boot_record; // 0
    uint8_t drive_number;          // 36
    uint8_t reserved0;             // 37 Flags in Windows NT. Reserved otherwise
    uint8_t signature;             // 38
    uint32_t volume_serial_id;     // 39
    uint8_t volume_label[11];      // 43
    uint8_t identifier[8];         // 54
    uint8_t boot_code[448];        // 62
    uint16_t boot_signature;       // 510
} __attribute__((packed)) fat16_boot_record_t;

#ifndef FAT121632
#define FAT121632

typedef struct fat_entry_s
{
    union
    {
        struct
        {
            uint8_t name[8];      // 0
            uint8_t extension[3]; // 8
        };
        uint8_t filename[11]; // 0
    };

    uint8_t attributes; // 11

    uint8_t reserved0;      // 12 Reserved for use by Windows NT.
    uint8_t time_to_create; // 13

    uint16_t time_created;       // 14    hour (5bits) minutes (6bits) seconds(5 bits) .... multiply seconds by 2
    uint16_t date_created;       // 16    year (7bits) month (4bits) day(5 bits)
    uint16_t last_accessed_date; // 18    year (7bits) month (4bits) day(5 bits)

    uint16_t cluster_num_high; // 20

    uint16_t last_modification_time; // 22    hour (5bits) minutes (6bits) seconds(5 bits) .... multiply seconds by 2
    uint16_t last_modification_date; // 24    year (7bits) month (4bits) day(5 bits)

    uint16_t cluster_num_low; // 26

    uint32_t file_size; // 28
} __attribute__((packed)) fat_entry_t;

typedef struct fat_file_entry_s
{
    fat_entry_t entry;
    uint32_t parent_block;
} __attribute__((packed)) fat_file_entry_t;

#endif

#endif