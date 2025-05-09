<h1> The Boot Process </h1>
<p> A kernel is loaded by a bootloader. This kernel specifically uses GRUB to load the kernel into 32-bit protected mode. </p>

<h2> GRUB 2 & Multiboot2 </h2>
    <p> GRUB 2 is a bootloader that loads and transfers control and information to the kernel. Multiboot 2 is a specfiication that describes or defines how GRUB passes information to the kernel.</p>
    <h3> Multiboot2 Headers | <code>multiboot-header.s</code> </h3> 
        <p> A Multiboot2 Header is a structure at the beginning of a kernel binary that lets GRUB 2 recognize and load the kernel. </p>

<h2> In Control </h2>
    <p> As previously stated, GRUB 2 loads the kernel into 32-bit protected mode. Once the kernel is loaded, it begins to execute.</p>
    <h3> A Higher-Half Kernel | <code>boot.s</code> </h3>
        <p>This kernel is a higher-half kernel. It runs in the higher-half of the virtual address space; it runs at the address <code>0xC0000000</code>.</p>
    <h3>How It's Done</h3>
        <p>In actuality, the kernel is initially loaded at a physical address (<code>0x100000</code>), which differs from the virtual address it will eventually run at.</p>
        <ol>
            <li> Enabling Paging </li>
                <p>The goal is to run the kernel at a higher virtual address. This is achieved through paging by defining a page directory, identity mapping the kernel's physical location, mapping the higher-half virtual address, loading the page directory, and then enabling paging.</p>
            <li> Jumping to the Higher Half </li>
                <p>Once paging is enabled, the kernel—still executing in the low memory area—jumps to its higher-half virtual address (e.g., <code>0xC0001000</code>). Because of the paging setup, this virtual address now correctly points to the physical location where the kernel is loaded.</p>
            <li> Invalidate Identity Mapping </li>
                <p> Since the kernel is running in the higher-half, the indentity-mapped entry must be removed. 
        </ol>

<h2> To C/C++ </h2>
    <p> Previously, everything was running in ASM. Lets be honest, that's no fun (in my opinion). So, how do we get to the fun? </p>
    <h3> !! Very Complicated !! </h3>
    <p> Ya, that's right: 
    <br>
    <code> 
    movl $stack_top, %esp
    <br>
    push %eax
    <br>
    push %ebx
    <br>
    call kernel_main 
    </code> 
    </p>