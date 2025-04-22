<h1> Interrupts </h1>
    <p> Interrupts are signals that temporarily halt the CPU's current operations to immediately address an important task or event. These interrupt handles are stored in the IDT.</p>
<h2> Interrupt Request </h2>
<p> An Interrupt Request (IRQ) is a hardware signal sent to the processor, such as the keyboard and mouse.</p>
<h3> Standard ISA IRQs </h3>
<table border="1" cellpadding="5" cellspacing="0">
  <thead>
    <tr>
      <th>IRQ</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>0</td><td>Programmable Interrupt Timer Interrupt</td></tr>
    <tr><td>1</td><td>Keyboard Interrupt</td></tr>
    <tr><td>2</td><td>Cascade (used internally by the two PICs, never raised)</td></tr>
    <tr><td>3</td><td>COM2 (if enabled)</td></tr>
    <tr><td>4</td><td>COM1 (if enabled)</td></tr>
    <tr><td>5</td><td>LPT2 (if enabled)</td></tr>
    <tr><td>6</td><td>Floppy Disk</td></tr>
    <tr><td>7</td><td>LPT1 / Unreliable "spurious" interrupt (usually)</td></tr>
    <tr><td>8</td><td>CMOS real-time clock (if enabled)</td></tr>
    <tr><td>9</td><td>Free for peripherals / legacy SCSI / NIC</td></tr>
    <tr><td>10</td><td>Free for peripherals / SCSI / NIC</td></tr>
    <tr><td>11</td><td>Free for peripherals / SCSI / NIC</td></tr>
    <tr><td>12</td><td>PS2 Mouse</td></tr>
    <tr><td>13</td><td>FPU / Coprocessor / Inter-processor</td></tr>
    <tr><td>14</td><td>Primary ATA Hard Disk</td></tr>
    <tr><td>15</td><td>Secondary ATA Hard Disk</td></tr>
  </tbody>
</table>

<h3> The Code </h3>
    <p> So, I am slightly too lazy to explain the code. </p>

<h2> Interrupt Service Routine </h2>
<p> Ya, I might give up on this (documentation) ngl.  </p>