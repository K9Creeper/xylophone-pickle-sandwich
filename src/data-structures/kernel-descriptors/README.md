<h1> A Kernel's Descriptor Tables Structures </h1>
    <p> Descriptor tables in general are 'arrays' or structures that hold information for about something. </p>
<h2> The Global Descriptor Table </h2>
    <p> The GDT (Global Descriptor Table) is a descriptor table (duh). Mainly, the GDT describes memory segments, most notably containing details about address start, size, access permissions.</p>
    <h3> The <code>Global_Descriptor_Table_Entry</code> </h3>
        <p> A GDT entry is only 8 bytes. </p>
        <ul>
        <li> <code>limit_low</code> </li>
            <p> The lower 16 bits of the segment limit, which defines the size of the segment. </p>
        <li> <code>base_low</code> </li>
            <p> The lower 16 bits of the base address where the segment begins in memory. </p>
        <li> <code>base_middle</code> </li>
            <p> The middle 8 bits of the base address, continuing from <code>base_low</code>. </p>
        <li> <code>access</code> </li>
            <p> The middle 8 bits of the base address, continuing from <code>base_low</code>. </p>
        <li> <code>granularity</code> </li>
            <p> Holds high 4 bits of the segment limit and flags for granularity, size, and long mode. </p>
        <li> <code>base_high</code> </li>
            <p> The highest 8 bits of the base address, the end of the 32-bit segment base. </p>
        </ul>
    <h3> The <code>Global_Descriptor_Table_Pointer</code> </h3>
        <p> The structure passed to the <code>lgdt</code> instruction.</p>
        <ul>
        <li> <code>limit</code> </li>
        <p> Specifies the size (minus one) of the entire GDT. </p>
        <li> <code>base</code> </li>
        <p> Contains the address where the GDT starts in memory. </p>
        </ul>
<h3> The Task State Segment </h3>
    <p> This is not a descriptor table. Why is it here?</p>
    <p> Well, the TSS (Task State Segment) is an descriptor (entry) in the GDT. It is used to store information about a task (or process), especially during task switching or when transitioning from user mode to kernel mode.</p>
<h2> The Interrupt Descriptor Table </h2>
    <p> The IDT (Interrupt Descriptor Table), like the GDT, is a Descriptor Table (duh). This descriptor table specifically acts as a table of interrupt vectors, where each element corresponds to an interrupt handle.</p>
    <h3> The <code>Interrupt_Descriptor_Table_Entry</code> </h3>
        <p> A IDT entry is only 8 bytes. </p>
        <ul>
        <li> <code>base_low</code> </li>
        <p> The lower 16 bits of the address of the interrupt service routine (ISR). </p>
        <li> <code>selector</code> </li>
        <p> The code segment selector in the GDT that the CPU will use when the ISR is called. </p>
        <li> <code>always0</code> </li>
        <p> This byte must always be set to 0; it is reserved by the CPU. </p>
        <li> <code>flags</code> </li>
        <p> Contains type and attribute flags, such as gate type (interrupt/trap), privilege level, and present bit. </p>
        <li> <code>base_high</code> </li>
        <p> The upper 16 bits of the ISR address, the end of the 32-bit handler address. </p>
        </ul>
    <h3> The <code>Interrupt_Descriptor_Table_Pointer</code> </h3>
        <p> The structure passed to the <code>lidt</code> instruction.</p>
        <ul>
        <li> <code>limit</code> </li>
        <p> Specifies the size (minus one) of the entire IDT. </p>
        <li> <code>base</code> </li>
        <p> Contains the address where the IDT starts in memory. </p>
        </ul>