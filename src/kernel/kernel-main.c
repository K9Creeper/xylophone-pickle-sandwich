/// -------------
/// kernel-main.c
/// @brief This is the C entry of the kernel.

#define cli() __asm__ volatile("cli")
#define sti() __asm__ volatile("sti")

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <multiboot2.h>

static void setup_misc(void);

static void setup_descriptors(void);

static void setup_bios32(void);

static void setup_early_heap(void);
static void setup_paging(void);
static void setup_heap(void);

void kernel_main(uint32_t addr, uint32_t magic)
{
    cli();

    setup_misc();

    setup_descriptors();

    setup_bios32();

    setup_early_heap();

    setup_paging();

    setup_heap();

    sti();

halt:
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

#include "misc-drivers/serial-com1.h"

void setup_misc(void)
{
    kernel_misc_drivers_serial_com1_init();
}

#include "descriptors/global-descriptor-table.h"
#include "descriptors/interrupt-descriptor-table.h"

void setup_descriptors(void)
{
    kernel_global_descriptor_table_init();
    kernel_global_descriptor_table_install();
    printf("Setup GDT\n");

    kernel_interrupt_descriptor_table_init();
    kernel_interrupt_descriptor_table_install();
    printf("Setup IDT\n");
}

#include "bios32/bios32.h"

void setup_bios32(void)
{
    kernel_bios32_init();
    printf("Setup Bios32\n");
}

#include "memory-management/kheap.h"
void setup_early_heap(void)
{
    kheap_pre_init();
}

#include "memory-management/pmm.h"
#include "memory-management/paging.h"
void setup_paging(void)
{
    pmm_init(0x1000000);
    paging_init();
}

void setup_heap(void)
{
    kheap_init();
}