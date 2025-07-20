/// -------------------
/// paging-structures.h
/// @brief This file contains the definition of paging-related data structures.

#ifndef PAGING_STRUCTURES_H
#define PAGING_STRUCTURES_H

#include <stdint.h>

typedef struct page_directory_entry_s
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t w_through : 1;
    uint32_t cache : 1;
    uint32_t access : 1;
    uint32_t reserved : 1;
    uint32_t page_size : 1;
    uint32_t global : 1;
    uint32_t available : 3;
    uint32_t frame : 20;
} page_directory_entry_t;

typedef struct page_entry_s
{
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t reserved : 2;
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t reserved2 : 2;
    uint32_t available : 3;
    uint32_t frame : 20;
} page_entry_t;

typedef struct page_table_s
{
    page_entry_t pages[1024];
} page_table_t;

typedef struct page_directory_s
{
    page_directory_entry_t tables[1024];
    page_table_t *ref_tables[1024];
} page_directory_t;

#endif