<h1> Interrupts (x86) </h1>
    <p> Interrupts are signals that temporarily halt the CPU's current operations to immediately address an important task or event. These interrupt handles are stored in the IDT.</p>
  
<h2> The Interrupt Service Routine (ISR) </h2>
  <p> A 'service' that executes a task ("task" in a literary sense) after reciving a Interrupt Request (IRQ). Think of the ISR as a handle listening for a ping by a IRQ. </p>

<h3> <code>Interrupt_Service_AddGates()</code> | <code>interrupt-service.cpp</code> </h3>
  <p> This function is really just a wrapper--to the <code> Interrupt_Descriptor_SetGate</code> function--setting the <a href="#EXCEPTIONS">exeption IDT gates (IDT Vectors 0-31)</a>, adding handles for each entry.<br/><br/>But how are these handles handled?</p>

<h3 id="ISR_HANDLE"> A ISR Handle | <code>interrupt-service.s</code> </h3>
  <p> Each ISR handle consists of the same strucure / concept. </p>
  <ol>
    <li> Clearing interrupts <br/> <code>cli</code> </li>
    <li> Pushing 0 to the stack <br/> <code>pushl $0</code> </li>
    <li> Pushing the vector number to the stack <br/> <code>pushl $V3CT0R_NUM83R</code> </li>
    <li> Jumping to the common ISR stub </li>
  </ol>

<h3> The <code>isr_common_stub</code> | <code>interrupt-service.s</code> </h3>
  <p> At the end of a <a href="#ISR_HANDLE">ISR handle's</a> execution the handle jumps to <code>isr_common_stub</code>.</p>

<h2> A Interrupt Request (IRQ) </h2>
  <p> A hardware signal sent to the processor, such as the keyboard and mouse. </p>

<h2 id="IDT_TABLES"> Interrupt Descriptor Table Gates </h2>

<h3 id="EXCEPTIONS"> Exceptions (IDT Vectors 0-31) </h3>
<table border="1" cellpadding="5" cellspacing="0">
  <thead>
    <tr>
      <th>Vector (IDT Index)</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>0</td><td>Divide Error</td></tr>
    <tr><td>1</td><td>Debug Exception</td></tr>
    <tr><td>2</td><td>Non-Maskable Interrupt (NMI)</td></tr>
    <tr><td>3</td><td>Breakpoint Exception</td></tr>
    <tr><td>4</td><td>Overflow Exception</td></tr>
    <tr><td>5</td><td>BOUND Range Exceeded Exception</td></tr>
    <tr><td>6</td><td>Invalid Opcode (Undefined Opcode)</td></tr>
    <tr><td>7</td><td>Device Not Available (No Math Coprocessor)</td></tr>
    <tr><td>8</td><td>Double Fault Exception</td></tr>
    <tr><td>9</td><td>Coprocessor Segment Overrun (reserved)</td></tr>
    <tr><td>10</td><td>Invalid TSS</td></tr>
    <tr><td>11</td><td>Segment Not Present</td></tr>
    <tr><td>12</td><td>Stack-Fault Exception</td></tr>
    <tr><td>13</td><td>General Protection Fault</td></tr>
    <tr><td>14</td><td>Page Fault</td></tr>
    <tr><td>15</td><td>Reserved</td></tr>
    <tr><td>16</td><td>Floating-Point Error (x87 FPU Floating-Point Exception)</td></tr>
    <tr><td>17</td><td>Alignment Check Exception</td></tr>
    <tr><td>18</td><td>Machine-Check Exception</td></tr>
    <tr><td>19</td><td>SIMD Floating-Point Exception</td></tr>
    <tr><td>20-31</td><td>Reserved...</td></tr>
  </tbody>
</table>

<h3> Programmable Interrupt Controller (PIC) IRQs (IDT Vectors 32-47) </h3>
<table border="1" cellpadding="5" cellspacing="0">
  <thead>
    <tr>
      <th>Vector (IDT Index)</th>
      <th>IRQ #</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>32</td><td>0</td><td>Programmable Interrupt Timer Interrupt</td></tr>
    <tr><td>33</td><td>1</td><td>Keyboard Interrupt</td></tr>
    <tr><td>34</td><td>2</td><td>Cascade (used internally by the two PICs, never raised)</td></tr>
    <tr><td>35</td><td>3</td><td>COM2 (if enabled)</td></tr>
    <tr><td>36</td><td>4</td><td>COM1 (if enabled)</td></tr>
    <tr><td>37</td><td>5</td><td>LPT2 (if enabled)</td></tr>
    <tr><td>38</td><td>6</td><td>Floppy Disk</td></tr>
    <tr><td>39</td><td>7</td><td>LPT1 / Unreliable "spurious" interrupt (usually)</td></tr>
    <tr><td>40</td><td>8</td><td>CMOS real-time clock (if enabled)</td></tr>
    <tr><td>41</td><td>9</td><td>Free for peripherals / legacy SCSI / NIC</td></tr>
    <tr><td>42</td><td>10</td><td>Free for peripherals / SCSI / NIC</td></tr>
    <tr><td>43</td><td>11</td><td>Free for peripherals / SCSI / NIC</td></tr>
    <tr><td>44</td><td>12</td><td>PS2 Mouse</td></tr>
    <tr><td>45</td><td>13</td><td>FPU / Coprocessor / Inter-processor</td></tr>
    <tr><td>46</td><td>14</td><td>Primary ATA Hard Disk</td></tr>
    <tr><td>47</td><td>15</td><td>Secondary ATA Hard Disk</td></tr>
  </tbody>
</table>