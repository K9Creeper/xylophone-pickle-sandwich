<h1> Memory Management Structures </h1>
    <h2> The Heap </h2>
        <p> The Heap manages virtual memory and provides dynamic memory allocation for the operating system. </p>
        <h3> Its Structures </h3>
            <p> The heap is managed by three different structures. </p>
            <h4> The <code>Heap_Header</code> </h4>
                <p> This structure lables the beginning of an allocated block. </p>
                <ul>
                    <li> <code>magic</code> </li>
                        <p> This 32-bit unsigned integer holds a magic number which helps identify the <code>Heap_Header</code> as a heap memory structure. </p>
                    <li> <code>is_hole</code> <li>
                        <p> This boolean holds whether or not this block holds a hole. </p>
                    <li> <code>size</code> <li>
                        <p> This 32-bit unsigned integer holds the size of the block (to the <code>Heap_Footer</code>). </p>
                </ul>
            <h4> The <code>Heap_Footer</code> </h4>
                <p> This structure lables the end of an allocated block. </p>
                <ul>
                    <li> <code>magic</code> </li>
                        <p> This 32-bit unsigned integer holds a magic number which helps identify the <code>Heap_Header</code> as a heap memory structure. </p>
                    <li> <code>header</code> </li>
                        <p> This pointer points to the <code>Heap_Header</code> that is at the start of this allocated block. </p>
                </ul>
            <h4> The <code>Heap_Array</code> | Dervies from <code>Ordered_Array<<z/>void*></code></h4>
                <p> This structure simulates the allocated blocks of heap. Each entry holds one of the heap memory structures above.</p>
                <ul><strong> There is no further required knowledge on information about the <code>Heap_Array</code>.</strong></ul>
    <h2> The Physical Memory Manager (PMM) </h2>
        <p> The Physical Memeory Manager manages a memory map of the system, as well as the allocation of physical memory frames of a paging manager. </p>
        <h3> Its Core Structure </h3>
            <p> The PMM is made up of one main component. </p>
            <h4> The <code>Physical_Memory_Manager_Bitmap</code> | Derives from <code>Bitmap<<z/>uint32_t></code></h4>
                <p> This structure holds information about the allocation of physical memory frames of a paging manager. </p>
    <h2> Paging </h2>
        <div>
        <h4> This may be slightly tricky, but here we go: </h4>
            <ol>
                    <li>You’ve got a tiny table that only fits 4 pizza slices at once. But you're craving a massive 100-slice pizza! You try to stack them all, but there’s no room—you start dropping slices, making a mess.</li>
                    <li>Enter Paging Pete, a wizard. “No need to cram all 100 slices at once,” he says. “I’ll keep 4 on the table. When you finish one, I’ll swap it out for the next one you need.”</li>
                    <li>With Pete's help, your table always has the right slices in front of you. You feel like you’ve got the whole pizza at once—even though your table never grew. It’s just being managed smarter.</li>
            </ol>
        </div>
        <p>That’s paging. Your system pretends it has tons of memory by swapping pieces in and out exactly when needed.</p>
        <h3> Its Structures (32-bit) </h3>
             <p> Paging is managed by four core structures. These are used to define how memory is divided into pages and how virtual memory is mapped to physical memory. </p> 
             <h4> The <code>Page_Directory_Entry</code> </h4>
                <p> This structure represents a single entry in the page directory, which points to a page table in memory. </p> 
                <ul> 
                    <li> <code>present</code> </li> 
                        <p> A 1-bit flag that determines if the page table is present in memory. </p>
                    <li> <code>rw</code> </li>
                        <p> A 1-bit flag indicating whether the pages are read/write (1) or read-only (0). </p>
                    <li> <code>user</code> </li>
                        <p> A 1-bit flag that allows user-mode access if set; kernel-only if clear. </p>
                    <li> <code>w_through</code> </li>
                        <p> Controls write-through caching for the page table. </p>
                    <li> <code>cache</code> </li>
                        <p> If set, caching is disabled for the page table. </p>
                    <li> <code>access</code> </li>
                        <p> Set by the CPU when the corresponding page table is accessed. </p>
                    <li> <code>reserved</code> </li>
                        <p> Reserved for CPU use. </p>
                    <li> <code>page_size</code> </li>
                        <p> If set, pages are 4MB in size instead of the usual 4KB. </p>
                    <li> <code>global</code> </li>
                        <p> Used with the global page feature to prevent TLB invalidation on context switch. </p>
                    <li> <code>available</code> </li>
                        <p> 3 bits available for system programmer use. </p>
                    <li> <code>frame</code> </li>
                        <p> The 20 most significant bits of the physical address of the page table (aligned to 4KB). </p>
                </ul> 
            <h4> The <code>Page_Entry</code> </h4> 
                <p> This structure represents a single page entry in a page table, mapping to a physical frame in memory. </p> 
                    <ul> <li> <code>present</code> </li> 
                        <p> A 1-bit flag that determines if the page is present in physical memory. </p>
                        <li> <code>rw</code> </li>
                            <p> A 1-bit flag indicating read/write access. </p>
                        <li> <code>user</code> </li>
                            <p> Determines user-mode access if set. </p>
                        <li> <code>reserved</code> </li>
                            <p> Reserved bits for future or architecture-specific use. </p>
                        <li> <code>accessed</code> </li>
                            <p> Set by the CPU when the page is accessed. </p>
                        <li> <code>dirty</code> </li>
                            <p> Set by the CPU when the page has been written to. </p>
                        <li> <code>reserved2</code> </li>
                            <p> Additional reserved bits for CPU use. </p>
                        <li> <code>available</code> </li>
                            <p> 3 bits available for system programmer use. </p>
                        <li> <code>frame</code> </li>
                            <p> The 20 most significant bits of the physical frame address this page maps to. </p>
            </ul> 
            <h4> The <code>Page_Table</code> </h4> 
                <p> This structure represents a page table containing 1024 <code>Page_Entry</code> objects. Each entry maps to a 4KB page, thus the table maps 4MB of memory. </p> 
                <ul> 
                    <li> <code>pages[1024]</code> </li> <p> An array of <code>Page_Entry</code> structures mapping virtual pages to physical frames. </p> 
                </ul> 
            <h4> The <code>Page_Directory</code> </h4> 
                <p> This structure represents the full page directory, which holds all the page tables required to manage a 4GB virtual address space. </p> 
                <ul> 
                    <li> <code>tables[1024]</code> </li> 
                        <p> An array of <code>Page_Directory_Entry</code> objects referencing physical addresses of page tables. </p>
                    <li> <code>ref_tables[1024]</code> </li>
                        <p> An array of pointers to the corresponding <code>Page_Table</code> structures in memory. Used by the OS to access tables directly. </p>
                </ul>
<h1> Memory Management Functionality (Overview) </h1>
    <h2> Working in Unison </h2>
        <p> All of these Structures work in unision, as previously states, paging links the heap with the system wide physical memory manager. But how does this actually work in the code? </p>
        <h3> <code>Paging_Manager</code> Initializations </h3>
            <p>Well, during the initialization of the <code>Paging_Manager</code>, before a heap can even function properly, the <code>Paging_Manager</code> utilizes a early or pre-initalization of the heap. Specifically, the the early heap, allocates or increments a "unordered" placement of memory. This allocation will hold important data of the <code>Paging_Manager</code>. But remember, this description really only applies to the initalization of a <code>Paging_Manager</code>.</p>
        <h3> Memory Allocation (and Page Allocation)</h3>
            <p> Memory Allocation is interesting... Once a heap is initialized and linked to a <code>Paging_Manager</code>, each allocation made by the heap is required to be mapped by the <code>Paging_Manager</code>. But also, when the <code>Paging_Manager</code> needs to expand (allocate a new <code>Page_Table</code>) the heap also needs to increase.</p>