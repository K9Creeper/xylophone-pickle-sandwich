<h1> Serial Communication (Ports) </h1>
    <p> Serial communication ports and communiction is send and receive data between a computer and external devices one bit at a time. </p>
    <h2> COM1 </h2>
    <p> COM1 is a serial communication port; in this case, COM1 is specifically being used to debug and produce a string output. </p>
    <h3> Setting COM1 up </h3>
    <p> The COM1 serial port is located at the I/O port base address <code>0x3F8</code>.</p>
    <ol>
        <li> Disabling Interrupts | <code>outportb(COM1_PORT_ADDRESS + 1, 0x00)</code> </li>
        <p> Offset 1 = Interrupt Enable Register (IER). Basically, writing <code>0x00</code> to this address disabled all UART interrupts. </p>
        <li> Enable DLAB | <code>outportb(COM1_PORT_ADDRESS + 3, 0x80)</code> </li>
        <p> Offset 3 = Line Control Register (LCR). Setting bit 7 (<code>0x80</code>) enables access to write the baud rate divisor.</p>
        <li> Set Baud Rate (Low Byte) | <code>outportb(COM1_PORT_ADDRESS + 0, 0x03)</code> </li>
        <p> Offset 0 = Divisor Latch Low Byte (DLL) when DLAB is set. Writing <code>0x03</code> sets the baud rate divisor low byte. <br> Baud rate = 115200 / divisor or 115200 / 3 = 38400 baud.</p>
        <li> Set Baud Rate (High Byte) | <code>outportb(COM1_PORT_ADDRESS + 1, 0x00)</code> </li>
        <p> Offset 1 = Divisor Latch High Byte (DLM) when DLAB is set. Writing <code>0x00</code> completes the previous writing of the baud rate. </p>
        <li> Use 8N1 format | <code>outportb(COM1_PORT_ADDRESS + 3, 0x03)</code> </li>
        <p> Offset 3 = Line Control Register (LCR). Writing <code>0x03</code> allows for 8-bit word length, no parity, and 1 stop bit. </p>
        <li> Enable FIFO | <code>outportb(COM1_PORT_ADDRESS + 2, 0xC7)</code></li>
        <p> Offset 2 = FIFO Control Register (FCR). Writing <code>0xC7</code> enable FIFOs (and clears TX/RX). </p>
        <li> Prepare the UART | <code>outportb(COM1_PORT_ADDRESS + 4, 0x0B)</code></li>
        <p> Offset 4 = Modem Control Register (MCR). Writing <code>0x0B</code> sets DTR (Data Terminal Ready), RTS (Request to Send), and OUT2 (must be set to allow interrupts); this esentially enables IRQs. </p>
    </ol>
    <h3> Writting through COM1 </h3>
    <p> Not that difficult: <code>outportb(COM1_PORT_ADDRESS, DATA)</code>.</p>