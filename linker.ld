ENTRY(_start)

phys = 0x100000;
offset = 0xC0000000;
virt = offset + phys;

SECTIONS
{
    .bios32_data 0x8000 : AT(0x8000)
    {
        __bios32_data_start = .;
        KEEP(*(.bios32_data))
        __bios32_data_end = .;
    }

    . = virt;

    __start_of_kernel_space = offset;
    __start_of_kernel = .;

    __kernel_heap_start = 0xC0400000;
    __kernel_heap_initial_end = 0xC0500000;
    __kernel_heap_max_end = 0xCFFFFF00;

    .text ALIGN(4K) : AT(ADDR(.text) - offset)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - offset)
    {
        *(.rodata)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - offset)
    {
        *(COMMON)
        *(.bss)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - offset)
    {
        *(.data)
    }
    
    .syscalls ALIGN(4) : AT(ADDR(.syscalls) - offset)
    {
        __start_syscalls = .;
        KEEP(*(.syscalls))
        __stop_syscalls = .;
    }

    .kthread ALIGN(4) : AT(ADDR(.kthread) - offset)
    {
        __start_kthread = .;
        KEEP(*(.kthread))
        __stop_kthread = .;
    }

    __end_of_kernel = .;
}