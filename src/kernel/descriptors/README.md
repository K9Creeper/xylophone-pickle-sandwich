<h1> Kernel Descriptors </h1>
    <h2> Global Descriptor Table </h2>
        <p> The structure of <code>Global_Descriptor_Table_Entry</code> and <code>Global_Descriptor_Table_Pointer</code> are explained in the <code>data-structures</code> folder and will not be explained here. </p>
        <h3> Global Descriptor Table Segments (32-bit) </h3>
        <ul>
            <li> Null Descriptor </li>
            <p> Note: This is the first entry in the GDT. It should always contain no data. </p>
            <ul>
                <li> Offset = <code>0x0000</code> </li>
                <li> Base = <code>0x0</code> </li>
                <li> Limit = <code>0x00000000</code> </li>
                <li> Access Byte = <code>0x00</code></li>
                <li> Flags = <code>0x0</code> </li>
            </ul>
            <li> Kernel Mode Code Segment </li>
            <ul>
                <li> Offset = <code>0x0008</code> </li>
                <li> Base = <code>0x0</code> </li>
                <li> Limit = <code>0xFFFFF</code> </li>
                <li> Access Byte = <code>0x9A</code></li>
                <li> Flags = <code>0xC</code> </li>
            </ul>
            <li> Kernel Mode Data Segment </li>
            <ul>
                <li> Offset = <code>0x0010</code> </li>
                <li> Base = <code>0x0</code> </li>
                <li> Limit = <code>0xFFFFF</code> </li>
                <li> Access Byte = <code>0x92</code></li>
                <li> Flags = <code>0xC</code> </li>
            </ul>
            <li> User Mode Code Segment </li>
            <ul>
                <li> Offset = <code>0x0018</code> </li>
                <li> Base = <code>0x0</code> </li>
                <li> Limit = <code>0xFFFFF</code> </li>
                <li> Access Byte = <code>0xFA</code></li>
                <li> Flags = <code>0xC</code> </li>
            </ul>
            <li> User Mode Data Segment </li>
            <ul>
                <li> Offset = <code>0x0020</code> </li>
                <li> Base = <code>0x0</code> </li>
                <li> Limit = <code>0xFFFFF</code> </li>
                <li> Access Byte = <code>0xF2</code></li>
                <li> Flags = <code>0xC</code> </li>
            </ul>
        </ul>
        <h3> Loading the GDT </h3>
        <ol>
            <li> Loading the GDT Pointer </li>
            <p> This is not the only step! First just load the GDT pointer using the instruction <code>lgdt</code>.</p>
            <li>Setting Segment Registers</li>
            <p> After loading the GDT, you must reload the segment registers to use the new GDT entries. Look at lines 10-15 in <code>global-descriptor-table.s</code>. Note: <code>%ax</code> is set as the offset of the kernel mode data segment (<code>0x10</code>). </p>
            <li>Far Jump to Reload Code Segment</li>
            <p> You can’t just move a new value into <code>%cs</code> (code segment). So, a far jump is used with an offset of the kernel mode code segment (<code>0x08</code>).</p>
        </ol>
    <h2> Task State Segment </h2>
        <p> As discussed in the <code>kernel-descriptors</code> folder, the TSS is a segment within the GDT. </p>
        <h3> The Segment Itself </h3>
        <ul>
            <li> Task State Segment </li>
            <ul>
                <li> Offset = <code>0x0028</code> </li>
                <li> Base = <code>&TSS</code> (pointer of Task_State_Segment_Entry) </li>
                <li> Limit = <code>sizeof(TSS)-1</code> (size of Task_State_Segment_Entry minus one)</li>
                <li> Access Byte = <code>0x89</code></li>
                <li> Flags = <code>0x0</code> </li>
            </ul>
        </ul>
    <h2> Interrupt Descriptor Table </h2>
        <p> The structure of <code>Interrupt_Descriptor_Table_Entry</code> and <code>Interrupt_Descriptor_Table_Pointer</code> are explained in the <code>data-structures</code> folder and will not be explained here. </p>
        <h3> The Gates </h3>
            <p> IDT Gates are discussed in the <code>interrupt</code> folder.</p>
        <h3> Loading the IDT </h3>
            <p> It is fairly simple. Only the <code>lidt</code> instruction is used to load the IDT pointer.</p>