ENTRY(_start)

phys = 0x100000;
offset = 0xC0000000;
virt = offset + phys;

SECTIONS
{
    . = virt;
    linkerld_startofkernel = .;

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
    
    linkerld_endofkernel = .;
}