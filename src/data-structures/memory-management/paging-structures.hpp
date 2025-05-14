/// ---------------------
/// paging-structures.hpp
/// This file contains the definition of paging-related data structures.
#pragma once

#include <stdint.h>

struct Page_Directory_Entry
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
};

struct Page_Entry
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
};

struct Page_Table
{
    Page_Entry pages[1024];
};

struct Page_Directory
{
    Page_Directory_Entry tables[1024];
    Page_Table *ref_tables[1024];
};